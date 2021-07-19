---
title: macOS Build
---

This document describes how to install and build SUMO on macOS from its source code. If you don't want to **extend** SUMO, but just **use** it, you may want to simply follow the [installation instructions for macOS](index.md#macos) instead.

You may use one of two ways to build and install SUMO on macOS: **Homebrew** (recommended) and **MacPorts**.

# The Homebrew Approach

## Prerequisites

The installation requires [Homebrew](http://brew.sh). If you did not already install homebrew, do so by invoking
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```
and make sure your homebrew db is up-to-date.
```
brew update
```

In order to compile the C++ source code files of SUMO, a C++ compiler is needed. On macOS the default C/C++ compiler is Clang. If you want to install the Clang compilers, please use the following command:
```
xcode-select --install
```
After the successful installation, you can test Clang with the following command:
```
clang --version
```

SUMO uses [CMake](https://cmake.org/) to manage the software compilation process. You can install CMake with homebrew easily.
```
brew install cmake
```

## Dependencies
In order to compile and execute SUMO, there are several libraries that need to be installed. You can install these dependencies with homebrew with the following commands:
```
brew install --cask xquartz
brew install xerces-c fox proj gdal gl2ps
```
Depending on the SUMO features you want to enable during compilation, you may want to additional libraries. Most libraries are available in homebrew and should be recognized with CMake. Here is what you need to prepare some more features such as libsumo and the testing environment:
```
brew install python swig eigen pygobject3 gtk+3 adwaita-icon-theme
python3 -m pip install texttest
```

### Troubleshooting the fox library on macOS Catalina
If you encounter segmentation faults on macOS Catalina, please follow the following steps (see [Issue 6242](https://github.com/eclipse/sumo/issues/6242#issuecomment-553458710)).

1. Uninstall Catalina bottle of fox:
```brew uninstall --ignore-dependencies fox```

2. Edit brew Formula of fox:
```brew edit fox```

3. Comment out or delete the following line:
```sha256 "c6697be294c9a0458580564d59f8db32791beb5e67a05a6246e0b969ffc068bc" => :catalina```

4. Install Mojave bottle of fox:
```brew install fox```

## Git Cloning and Building
The source code of SUMO can be cloned with the following command to the directory `./sumo`. The environment variable `SUMO_HOME` should also be set to this directory.
```
git clone --recursive https://github.com/eclipse/sumo
export SUMO_HOME="$PWD/sumo"
```
SUMO is usually build as an out-of-source build. You need to create a directory for your build and invoke CMake to trigger the configuration from there. 
```
cd $SUMO_HOME
mkdir build/cmake-build
cd build/cmake-build
cmake ../..
```
The output of the CMake configuration process will show you which libraries have been found on your system and which SUMO features have been enabled accordingly.
The build process can now be triggered with the following command
```
cd $SUMO_HOME/build/cmake-build
cmake --build . --parallel $(sysctl -n hw.ncpu)
```
## Optional Steps

### TraCI as a Service (TraaS) 
TraaS is a java library for working with TraCI. Building TraaS can be triggered with the following commands.
```
cd $SUMO_HOME/build/cmake-build
cmake --build . --target traas --parallel
```

### Examples and Unit Tests
SUMO provides unit tests to be used with Google's Testing and Mocking Framework - Googletest. In order to execute these tests you need to install Googletest first.
```
git clone https://github.com/google/googletest
cd googletest
git checkout release-1.10.0
mkdir build
cd build
cmake ..
make
make install
```

The creation of the examples and the execution of the tests can be triggered as follows
```
cd $SUMO_HOME/build/cmake-build
make CTEST_OUTPUT_ON_FAILURE=1 examples test
```

More information is provided [here](../Developer/Unit_Tests.md).

### Integration Tests with TextTest
SUMO uses an application called TextTest to manage and execute and extensive set of integration tests. If you plan to extend SUMO with new features, we would like to encourage you to also add tests for your code to the SUMO testsuite and to make sure that existing functionality is not affected. 

The installation of TextTest on macOS is documented [here](../Developer/Tests.md).

### Code Editor
Finally, you may also want to use a code editor or integrated development environment. There is a great variety of suitable tools available. If you are unsure which tool to pick, we would suggest to have a look at [Visual Studio Code](https://code.visualstudio.com/) for macOS. The configuration of Visual Studio Code for the CMake setup is documented [here](../Developer/VisualStudioCode.md).

### Python for libsumo on macOS
There are usually three Python versions available on a mac with brew.
- Python 2.7 (shipped with macOS) - (`/usr/bin/python`)
- Python 3.8.2 (shipped with Xcode) - (`/usr/bin/python3`)
- Python 3.8.6 (installed with Brew) - (`/usr/local/bin/python3.8`)

If you do not configure anything "special" during `cmake`, it will find the Python 2.7 installation and use that version of the Python framework for building libsumo. In your cmake output, you will find something like this:
```
-- Found PythonInterp: /usr/bin/python (found version "2.7.16") 
-- Found Python: /usr/bin/python
...
-- Found PythonLibs: /usr/lib/libpython2.7.dylib (found version "2.7.16") 
```
You should be able to just use python 2.7 and import libsumo.
```
Python 2.7.16 (default, Jun  5 2020, 22:59:21) 
[GCC 4.2.1 Compatible Apple LLVM 11.0.3 (clang-1103.0.29.20) (-macos10.15-objc- on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> import libsumo
>>>
```

If you want to use Python **3.x**, we recommend to use the Python version shipped with Brew (e.g. `brew install python@3.8`). In order to use this Python version, you have to tell CMake to use this particular Python interpreter:
```
cd $SUMO_HOME
mkdir -p out/build-python3.8
cd out/build-python3.8
cmake -DPYTHON_EXECUTABLE=/usr/local/bin/python3.8 ../..
```
In the CMake configuration output, you should see lines like the following:
```
-- Found PythonInterp: /usr/local/bin/python3.8 (found version "3.8.6")
... 
-- Found PythonLibs: /usr/local/Frameworks/Python.framework/Versions/3.8/lib/libpython3.8.dylib (found version "3.8.6") 
```
After building, you should be able to load `libsumo`
```
$ python3.8                                                                                                           
Python 3.8.6 (default, Oct 27 2020, 08:57:44) 
[Clang 12.0.0 (clang-1200.0.32.21)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> import libsumo
>>> 
```

If you do not want to use Python 2.7 (from macOS) or Python 3 (from Brew), but instead want to use Python 3.x from Xcode, you will need to tell `cmake` where to find the Python installation of the command line tools of Xcode. The following path settings work on macOS Catalina with the command line tools of Xcode 12.1.
```
cmake \
-DPYTHON_EXECUTABLE=/usr/bin/python3 \
-DPYTHON_INCLUDE_DIR:PATH=/Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Headers \
-DPYTHON_LIBRARY:FILEPATH=/Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Versions/Current/lib/libpython3.8.dylib \
../..
```
After the build, you should be able to load libsumo.
```
/usr/bin/python3
Python 3.8.2 (default, Sep 24 2020, 19:37:08) 
[Clang 12.0.0 (clang-1200.0.32.21)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> import libsumo
>>> 
```

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

After obtaining the [required libraries](Linux_Build.md#installing_required_tools_and_libraries)
you can follow the build steps of [building under Linux](Linux_Build.md#getting_the_source_code), you
might want to add another --prefix=/opt/sumo to the configure line.

If you wish to use clang rather than gcc for compilation do:

```
./configure CXX=clang++ CXXFLAGS="-stdlib=libstdc++"
```
