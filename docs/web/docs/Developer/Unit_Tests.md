---
title: Unit Tests
---

# Introduction

The Unit Tests for SUMO are carried out with the help of the Framework
[Google Test](https://github.com/google/googletest). With *Google Test*
new Unit Tests can be simple added or existing tests extended. All
available tests are executed daily and it is checked whether these tests
run through successfully. This installation guide is for the version
1.4.0 to 1.12.1 of *Google Test*. SUMO is not compatible with Google Test 1.13 (or later)
yet, because this requires C++14.

# Windows Setup

Please clone the
[SUMOLibraries](https://github.com/DLR-TS/SUMOLibraries) and set the
environment variable `SUMO_LIBRARIES` to the checkout directory. Google
Test should be found and set up automatically then.

# Linux setup

For Ubuntu 22.04 a simple `sudo apt install libgtest-dev` suffices.

For older Ubuntu versions follow [this
recipe](https://stackoverflow.com/questions/24295876/cmake-cannot-find-googletest-required-library-in-ubuntu),
which boils down to:

```
sudo apt install libgtest-dev build-essential cmake
cd /usr/src/googletest
sudo cmake .
sudo cmake --build . --target install
```
