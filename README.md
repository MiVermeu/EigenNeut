# EigenNeut
Make art using neutrino oscillations! Heavily based on HackerPoet's [**Conics**](https://github.com/HackerPoet/Conics).

## Controls
* Mouse - Drag the sliders to vary neutrino oscillation parameters.
* Space Bar - Animate last altered oscillation parameter.
* Enter - Input custom value for last altered oscillation parameter.
* Left/Right - Switch between electron, muon and tau neutrino.
* e, l, x - Export oscillation probabilities to csv as a function of energy, length, or the last altered parameter.
* a - Toggle between neutrino and antineutrino oscillation.
* m - Toggle mass hierarchy.
* Escape - Exit the app.

## Dependencies
`CMake`, `SFML 2` and `Eigen 3`

macOS:
* Install the [**Homebrew**](https://brew.sh) package manager.
* `brew install cmake`
* `brew install sfml`
* `brew install eigen`

Ubuntu:
* `sudo apt-get install cmake`
* `sudo apt-get install libsfml-dev`
* `sudo apt-get install libeigen3-dev`

## Building

* `mkdir build`
* `cd build`
* `cmake ..`
* `make`