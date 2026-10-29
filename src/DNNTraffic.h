/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * Volume and packet arithmetic for the DNN layer traffic model.
 *
 * Kept free of SystemC on purpose so it can be unit tested on its own
 * (see other/regression/dnn_volume_test.cpp).
 */

#ifndef __NOXIMDNNTRAFFIC_H__
#define __NOXIMDNNTRAFFIC_H__

// Data volumes of one Conv2D layer, in values.
struct DNNLayerVolumes {
    long long weights;   // K * C * R * S
    long long inputs;    // C * H * W
    long long outputs;   // K * out_h * out_w
};

DNNLayerVolumes dnnLayerVolumes(int input_channels, int output_channels,
                                int input_h, int input_w,
                                int kernel_size, int stride);

// Values assigned to part k (0 <= k < n) when `total` values are split
// across n parts. The first total % n parts carry one extra value, so the
// shares always add up to exactly `total`.
long long dnnShare(long long total, int n, int k);

// Packets needed to carry `values` values at `vpp` values per packet,
// rounded up. A part with no values needs no packets.
long long dnnPackets(long long values, int vpp);

#endif
