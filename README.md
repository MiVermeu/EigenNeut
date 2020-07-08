# EigenNeut
Make art using neutrino oscillations! Heavily based on HackerPoet's [**Conics**](https://github.com/HackerPoet/Conics).

## Key Bindings
* Mouse - Drag the sliders to vary neutrino oscillation parameters.
* Space Bar - Animate last altered oscillation parameter.
* Left/Right - Switch between electron, muon and tau neutrino.
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
