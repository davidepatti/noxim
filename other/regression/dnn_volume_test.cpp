/*
 * Volume-conservation tests for the DNN layer traffic model.
 *
 * Checks that splitting a layer's weights, inputs and outputs across the
 * compute PEs never drops values, and that the packet schedule built from
 * those shares has enough capacity to carry the whole layer.
 *
 * Build and run (regression.sh does this automatically):
 *   g++ -std=c++11 -Wall -I src other/regression/dnn_volume_test.cpp \
 *       src/DNNTraffic.cpp -o dnn_volume_test && ./dnn_volume_test
 */

#include "DNNTraffic.h"

#include <cstdio>

static int failures = 0;
static int checks = 0;

#define CHECK(cond, ...)                                    \
    do {                                                    \
        ++checks;                                           \
        if (!(cond)) {                                      \
            ++failures;                                     \
            std::printf("FAIL %s:%d: ", __FILE__, __LINE__); \
            std::printf(__VA_ARGS__);                       \
            std::printf("\n");                              \
        }                                                   \
    } while (0)

// One direction of traffic: `total` values split over n PEs, packetized at
// vpp values per packet. Mirrors ProcessingElement::generateDNNSchedule().
static void checkStream(const char *what, long long total, int n, int vpp,
                        long long *packets_out)
{
    long long sum_values = 0;
    long long sum_packets = 0;
    long long min_share = total, max_share = 0;

    for (int k = 0; k < n; k++) {
        long long share = dnnShare(total, n, k);
        long long packets = dnnPackets(share, vpp);

        CHECK(share >= 0, "%s: negative share %lld for PE %d", what, share, k);
        // Each PE's packets carry its share, with less than one packet of padding.
        CHECK(packets * vpp >= share,
              "%s: PE %d gets %lld values but only %lld packets of %d",
              what, k, share, packets, vpp);
        CHECK(packets * vpp - share < vpp,
              "%s: PE %d over-allocated (%lld values, %lld packets of %d)",
              what, k, share, packets, vpp);
        CHECK(share > 0 || packets == 0,
              "%s: PE %d has no values but %lld packets", what, k, packets);

        if (share < min_share) min_share = share;
        if (share > max_share) max_share = share;
        sum_values += share;
        sum_packets += packets;
    }

    // Conservation: nothing is lost before rounding to packets.
    CHECK(sum_values == total, "%s: shares add up to %lld, layer has %lld (n=%d)",
          what, sum_values, total, n);
    // Balance: shares differ by at most one value.
    CHECK(max_share - min_share <= 1, "%s: unbalanced shares %lld..%lld",
          what, min_share, max_share);
    // Capacity covers the whole volume.
    CHECK(sum_packets * vpp >= total, "%s: capacity %lld < volume %lld",
          what, sum_packets * vpp, total);

    if (packets_out) *packets_out = sum_packets;
}

static void checkLayer(int C, int K, int H, int W, int R, int S,
                       int mesh_x, int mesh_y, int vpp)
{
    char what[160];
    std::snprintf(what, sizeof what,
                  "C=%d K=%d H=%d W=%d R=%d s=%d mesh=%dx%d vpp=%d",
                  C, K, H, W, R, S, mesh_x, mesh_y, vpp);

    DNNLayerVolumes v = dnnLayerVolumes(C, K, H, W, R, S);
    int n = mesh_x * mesh_y - 1;
    if (n < 1) n = 1;

    // Memory -> compute: weights and inputs are split separately, then
    // combined per destination, exactly as the memory node schedules them.
    long long sum_values = 0;
    for (int k = 0; k < n; k++) {
        long long values = dnnShare(v.weights, n, k) + dnnShare(v.inputs, n, k);
        long long packets = dnnPackets(values, vpp);
        CHECK(packets * vpp >= values, "%s: memory->PE %d short", what, k);
        sum_values += values;
    }
    CHECK(sum_values == v.weights + v.inputs,
          "%s: memory stream carries %lld of %lld values",
          what, sum_values, v.weights + v.inputs);

    checkStream(what, v.weights, n, vpp, 0);
    checkStream(what, v.inputs,  n, vpp, 0);
    checkStream(what, v.outputs, n, vpp, 0);
}

int main()
{
    // The case from the review: 4x4 mesh, K=8, H=W=4, kernel=1, stride=1,
    // eight-flit packets. 128 outputs over 15 compute PEs used to be
    // truncated to 8 values per PE (capacity 120).
    {
        DNNLayerVolumes v = dnnLayerVolumes(1, 8, 4, 4, 1, 1);
        CHECK(v.outputs == 128, "expected 128 outputs, got %lld", v.outputs);
        long long packets = 0;
        checkStream("review case outputs", v.outputs, 15, 8, &packets);
        CHECK(packets * 8 >= 128, "review case capacity %lld < 128", packets * 8);
        long long sum = 0;
        for (int k = 0; k < 15; k++) sum += dnnShare(v.outputs, 15, k);
        CHECK(sum == 128, "review case shares add up to %lld, not 128", sum);
    }

    // Layer dimensions from the existing example configs.
    checkLayer(16, 32, 8, 8, 3, 1, 4, 4, 8);      // config_examples small layer
    checkLayer(48, 128, 27, 27, 5, 1, 8, 8, 8);   // AlexNet conv2 (Eyeriss)

    // Sweep: volumes smaller than, equal to and larger than the PE count,
    // with and without remainders, across mesh sizes and packet sizes.
    const int meshes[][2] = { {2, 2}, {3, 3}, {4, 4}, {5, 3}, {8, 8} };
    const int vpps[] = { 1, 2, 5, 8 };
    for (const auto &m : meshes)
        for (int vpp : vpps)
            for (int C = 1; C <= 5; C++)
                for (int K = 1; K <= 9; K += 4)
                    for (int H = 1; H <= 7; H += 3)
                        for (int R = 1; R <= H; R += 2)
                            for (int S = 1; S <= 2; S++)
                                checkLayer(C, K, H, H + 1, R, S, m[0], m[1], vpp);

    // Edge cases of the helpers themselves.
    CHECK(dnnShare(0, 5, 0) == 0, "zero total");
    CHECK(dnnShare(3, 5, 4) == 0, "fewer values than parts");
    CHECK(dnnShare(3, 5, 2) == 1, "remainder goes to the first parts");
    CHECK(dnnShare(7, 0, 0) == 0, "zero parts");
    CHECK(dnnPackets(0, 8) == 0, "zero values need zero packets");
    CHECK(dnnPackets(1, 8) == 1, "one value needs one packet");
    CHECK(dnnPackets(8, 8) == 1, "exact fit");
    CHECK(dnnPackets(9, 8) == 2, "round up");
    CHECK(dnnLayerVolumes(1, 1, 4, 4, 1, 0).outputs == 16, "stride 0 is guarded");

    if (failures) {
        std::printf("dnn_volume_test: %d of %d checks failed\n", failures, checks);
        return 1;
    }
    std::printf("dnn_volume_test: all %d checks passed\n", checks);
    return 0;
}
