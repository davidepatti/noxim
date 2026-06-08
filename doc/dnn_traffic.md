# DNN Layer Traffic Mode

## Overview

The `dnn_layer` traffic mode models the communication pattern produced
by a DNN accelerator executing a single convolutional layer with an
output-stationary dataflow. Unlike the synthetic patterns (random,
transpose, shuffle), this mode generates a deterministic, structured
traffic schedule derived from the layer dimensions.

## Model

The mesh is partitioned into two roles:

- Node 0 acts as the DRAM / memory interface.
- All other nodes act as compute processing elements (PEs).

Traffic flows in three logical phases, collapsed into a per-source
schedule:

1. Weight distribution: the memory node sends filter weights to every
   compute PE.
2. Activation streaming: the memory node sends input feature maps to
   the compute PEs.
3. Output collection: each compute PE sends its partial-sum outputs
   back to the memory node.

Each PE computes its own injection schedule based on its node id, so
the model is fully distributed with no central scheduler.

## Configuration

Add a `dnn_layer` block to the YAML config and set
`traffic_distribution: TRAFFIC_DNN_LAYER`.

    dnn_layer:
      input_channels: 16     # C
      output_channels: 32    # K
      input_h: 8             # H
      input_w: 8             # W
      kernel_size: 3         # R = S
      stride: 1
      dataflow: output_stationary

Run with:

    ./bin/noxim -config config_examples/default_config_dnn.yaml -power bin/power.yaml

## Data Volume Computation

Given a Conv2D layer, the module computes:

- Weights:     K * C * R * S
- Inputs:      C * H * W
- Outputs:     K * out_h * out_w

where out_h = (H - R) / stride + 1 and out_w = (W - S) / stride + 1.

These values are distributed evenly across the compute PEs and
converted into packets using the configured packet size.

## Validation

The module was validated using the AlexNet CONV2 layer dimensions from
Chen, Emer, and Sze, "Eyeriss: A Spatial Architecture for
Energy-Efficient Dataflow for Convolutional Neural Networks" (ISCA
2016): C=48, K=128, H=W=27, R=S=5, stride=1.

Comparison on an 8x8 mesh with XY routing:

    Metric                  DNN_LAYER      Random baseline
    Average delay (cycles)  ~312           ~25
    Throughput (flits/cyc)  ~0.59          ~5.17

The DNN pattern produces roughly 12x higher average delay and 8x lower
throughput than uniform random traffic. This is the expected result:
all compute PEs converge their output traffic on the single memory
node, creating a many-to-one bottleneck that mirrors the memory
bandwidth pressure seen in real DNN accelerators.

Seed stability was confirmed across five runs, with throughput holding
steady between 0.58 and 0.59 flits/cycle.

Full results are in other/studies/dnn_validation/results.txt.

## Limitations and Future Work

This initial version supports one layer type (Conv2D) and one dataflow
(output-stationary). Natural extensions include weight-stationary and
row-stationary dataflows, multicast weight distribution, and
multi-layer pipelines.
