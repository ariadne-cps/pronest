

# ProNest

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT) [![Unix Status](https://github.com/ariadne-cps/pronest/workflows/Unix/badge.svg)](https://github.com/ariadne-cps/pronest/actions/workflows/unix.yml)
[![Windows Status](https://github.com/ariadne-cps/pronest/workflows/Windows/badge.svg)](https://github.com/ariadne-cps/pronest/actions/workflows/windows.yml) [![codecov](https://codecov.io/gh/ariadne-cps/pronest/branch/main/graph/badge.svg)](https://codecov.io/gh/ariadne-cps/pronest)

ProNest is a library for handling nested properties.
It is meant to expose software properties whose values can be defined in a range. 
The purpose it to be able to obtain the set of all allowed values, to execute the software with multiple values and find the best one according to given metrics.

### Building

To build the library from sources in a clean way, it is preferable that you set up a build subdirectory, say:

```
$ mkdir build && cd build
```

Then you can prepare the build environment, choosing a Release build for maximum performance:

```
$ cmake .. -DCMAKE_BUILD_TYPE=Release
```

At this point, if no error arises, you can build with:

```
$ cmake --build .
```

The library is meant to be used as a dependency, in particular by disabling testing as long as the *tests* target is already defined in an enclosing project.

## Contribution guidelines ##

If you would like to contribute to ProNest, please contact the developer: 

* Luca Geretti <luca.geretti@univr.it>
