/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * Volume and packet arithmetic for the DNN layer traffic model.
 */

#include "DNNTraffic.h"

DNNLayerVolumes dnnLayerVolumes(int input_channels, int output_channels,
                                int input_h, int input_w,
                                int kernel_size, int stride)
{
    // checkConfiguration() rejects stride <= 0; guard anyway so this is
    // safe to call on its own.
    if (stride < 1) stride = 1;

    long long out_h = (input_h - kernel_size) / stride + 1;
    long long out_w = (input_w - kernel_size) / stride + 1;
    if (out_h < 1) out_h = 1;
    if (out_w < 1) out_w = 1;

    DNNLayerVolumes v;
    v.weights = (long long)output_channels * input_channels *
                kernel_size * kernel_size;
    v.inputs  = (long long)input_channels * input_h * input_w;
    v.outputs = (long long)output_channels * out_h * out_w;
    return v;
}

long long dnnShare(long long total, int n, int k)
{
    if (n < 1 || k < 0 || k >= n || total <= 0)
        return 0;
    long long base = total / n;
    long long rem  = total % n;
    return base + (k < rem ? 1 : 0);
}

long long dnnPackets(long long values, int vpp)
{
    if (values <= 0)
        return 0;
    if (vpp < 1) vpp = 1;
    return (values + vpp - 1) / vpp;
}
