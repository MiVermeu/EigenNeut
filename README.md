# EigenNeut
Make art using neutrino oscillations! Heavily based on HackerPoet's [**Conics**](https://github.com/HackerPoet/Conics).

## Key Bindings
* Space Bar - Start neutrino drawing, play/pause δ<sub>CP</sub> rotation.
* Left/Right - Switch between electron, muon and tau neutrino.
* Up/Down - Increment/decrement the δ<sub>CP</sub> parameter.
* Escape - Exit the app.

## Neutrino parameters
For now, you can only change the neutrino oscillation parameters by changing the hardcoded values in `src/NeutOsc.h`.

## Building

Dependencies: `SFML 2`, `Eigen 3`.

Building with CMake:

* `mkdir build`
* `cd build`
* `cmake ..`
* `make`
