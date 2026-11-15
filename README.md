# FAIR Data Pipeline C++ API

[![FDP C++ API](https://github.com/FAIRDataPipeline/cppDataPipeline/actions/workflows/fdp_cpp_api.yaml/badge.svg)](https://github.com/FAIRDataPipeline/cppDataPipeline/actions/workflows/fdp_cpp_api.yaml) 
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=FAIRDataPipeline_FDP-Cpp-API&metric=coverage)](https://sonarcloud.io/summary/new_code?id=FAIRDataPipeline_FDP-Cpp-API)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![software-checklist](https://img.shields.io/badge/software-checklist-3891A6)](https://github.com/FAIRDataPipeline/cppDataPipeline/blob/main/software-checklist.md)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5874669.svg)](https://doi.org/10.5281/zenodo.5874669)

## Contents
  - [Installation](#installation)
  - [Outline](#outline)
  - [Unit Tests](#unit-tests)

## Installation
You can build and test the library using CMake, this implementation requires `C++11`.

It is recomended that you install CURL prior to installation of this API
- [CURL](https://curl.se/libcurl/)

Compile the library and tests by running:

```
$ cmake -Bbuild -DFDPAPI_BUILD_TESTS=ON
$ cmake --build build
```

**Important** For multi-config compilers such as Visual Studio xcode or ninja the config type needs to be set to `Release` otherwise the API will not build for example:
```
$ cmake -Bbuild -DFDPAPI_BUILD_TESTS=ON
$ cmake --build build --config=Release
```

The library can be installed system-wide using:

```
$ cmake --build build --target install
```

After doing so, the library can be discovered and utilised in other CMake projects
using:

```
find_package(fdpapi)
target_link_libraries(my_project PRIVATE fdpapi::fdpapi)
```


## Outline
The main class the user will interact with is `DataPipeline` which has only the required methods such as `link_read` etc. This class has a member which is a pointer to an underlying `DataPipelineImpl_` class which performs the various procedures required to handle the data. A logger has been used to give as much feedback to the user as possible, the verbosity being handled by a log level argument.

### Logging
The environment variable `FDP_LOG_LEVEL=[TRACE:DEBUG:INFO:WARN:ERROR:CRITICAL:OFF]` can be set to specify the logging output level.

## Unit Tests
The unit tests use the local registry, this needs to be running prior to running the tests see: [the CLI documentation](https://github.com/FAIRDataPipeline/FAIR-CLI#registry)

Tests can be lauched using the following command:
### Unix
```
$ ./build/bin/fdpapi-tests
```

### Windows
```
$ build\bin\Release\fdpapi-tests.exe
```
