# Time-Varying Filters Verification for Raspberry Pi Pico 2

This project is a verification implementation of digital filters with time-varying parameters, specifically optimized for Raspberry Pi Pico 2 (RP2350) digital signal processing (DSP) tasks.

## Intent and Background

In musical applications, such as synthesizer filters modulated by LFOs or envelopes, filter parameters often change on a per-sample basis,. Depending on the filter structure, these rapid changes can lead to instability or audible artifacts known as "clicks",.
This project implements and compares various filter structures based on the theories presented in Aaron Wishnick's paper, "Time-Varying Filters for Musical Applications". The primary focus is to evaluate the suitability of the State Variable Filter (SVF) for musical use cases within the resource-constrained environment of the RP2350,.

## Supported Filter Structures

The following five structures are implemented for comparison:

- DF2 (Direct-Form II): A common structure that is prone to instability and large artifacts during rapid parameter changes,.
- GR (Coupled Form / Gold and Rader): Known for being BIBO stable in the time-varying case, though it often receives lower subjective quality ratings in musical contexts,.
- SVF (State Variable Filter): The proposed solution in the paper. It is time-varying stable and provides an ideal DC response,,.
- TDF2RC (Stabilized TDF-II): A structure stabilized using Rabenstein and Czarnach’s state vector transformation. It often achieves the highest subjective quality scores,.
- Ladder (Normalized Ladder): Mentioned in prior work. While it may have higher DC error, its transient response can "smooth out" parameter changes,.

## Test Methodology

Following the experimental setup described in the paper (Table 3, 4), the filters are evaluated using two primary metrics,:

1. Objective Evaluation (DC Stimulus): Measures the l2 norm error (in dB) when parameters are changed instantaneously while passing a steady DC signal,. The SVF is expected to achieve an ideal result of `-inf`,.
2. Perceptual Indicators (Sideband Energy): Analyzes the "Sideband energy" produced during parameter changes. This metric is negatively correlated with subjective MUSHRA scores (r=−0.59), meaning lower values indicate fewer audible "clicks" and higher audio quality.

## Build and Execution

To compile and install this project, a setup with the [pico-sdk](https://github.com/raspberrypi/pico-sdk) is necessary. Please refer to [Getting Started with Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) to prepare your toolchain.

```bash
git clone https://github.com/oyama/pico-time-varying-filter-test.git
cd pico-time-varying-filter-test
git submodule update --init
mkdir build; cd build
PICO_SDK_PATH=/path/to/pico-sdk cmake .. -DPICO_BOARD=pico2
make
```

After successful compilation, the `pico_time_varying_filter_test.uf2` binary will be produced. Simply drag and drop this file onto your Raspberry Pi Pico 2 while in BOOTSEL mode to install.
Execution results are output to USB Serial.

The output will display:

- l2 norm error (dB) for DC response.
- Transient Signal Variance.
- Sideband energy (RMS) as a perceptual indicator.

##  Implementation Details

All filters are normalized to `Q=6` for consistency with the paper's trials.

- SVF: Uses resonance parameter R=1/2Q.
- Ladder: Maps the resonance range (0–4.5) to a comparable Q factor.

## Reference

- Aaron Wishnick, "Time-Varying Filters for Musical Applications," Proc. of the 17th Int. Conference on Digital Audio Effects (DAFx-14), 2014.
