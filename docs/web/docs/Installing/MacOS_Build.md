---
title: Installing/MacOS Build
permalink: /Installing/MacOS_Build/
---

This document describes how to install and build SUMO on MacOS from its source code. If you don't want to **extend** SUMO, but just **use** it, you may want to simply follow the [installation instructions for MacOS](../Installing.md#macos) instead.

You may use one of two ways to build and install SUMO on MacOS: Homebrew (recommended) and MacPorts.

# The Homebrew Approach

## Prerequisites

### Homebrew
The installation requires [Homebrew](http://brew.sh). If you did not already install [homebrew](http://brew.sh), do so by
invoking

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

and make sure your homebrew db is up-to-date

```
brew update
```

### C++ Compiler
In order to compile the C++ source code files of SUMO, a C++ compiler is needed. On MacOS the default C/C++ compiler is Clang. If you want to install the Clang compilers, please use the following command:
```
xcode-select --install
```
After the successful installation, you can test `clang` with the following command:
```
clang --version
```

## Dependencies

* `brew install cmake`
* `brew cask install xquartz`
* `brew install xerces-c fox proj gdal gl2ps`


## Build SUMO
* ...

## Optional Steps

### TraaS 
* ...

### Examples and Tests
* Google Test 
* ...

## Install TextTest
* Link to TextTest installation

## Install Editor
* Link to VSCode documentation


# The Macports Approach (legacy)

!!! note
    This uses a pre-packaged version of sumo which is convenient but may lag behind the latest official release of SUMO.

You should start by [installing Macports](https://www.macports.org/install.php). Afterwards start a terminal session and run

```
sudo port install sumo
```

While this will install a SUMO version you maybe do not want to use, it will pull in all dependencies you need.

If you want to build from a repository checkout you should additionally do

```
sudo port install automake autoconf
```

After obtaining the [required libraries](../Installing/Linux_Build.md#installing_required_tools_and_libraries)
you can follow the build steps of [building under Linux](../Installing/Linux_Build.md#getting_the_source_code), you
might want to add another --prefix=/opt/sumo to the configure line.

If you wish to use clang rather than gcc for compilation do:

```
./configure CXX=clang++ CXXFLAGS="-stdlib=libstdc++"
```
