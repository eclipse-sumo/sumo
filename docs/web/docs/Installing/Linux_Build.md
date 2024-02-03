---
title: Linux Build
---

This document describes how to install SUMO on Linux from sources. If
you don't want to **extend** SUMO, but merely **use** it, you might want
to [download one of our pre-built binary
packages](index.md) instead.

To be able to run SUMO on Linux, just follow these steps:

1.  Install all of the required tools and libraries
2.  Get the source code
3.  Build the SUMO binaries

For ubuntu this boils down to

```
sudo apt-get install git cmake python3 g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev python3-dev swig default-jdk maven libeigen3-dev
git clone --recursive https://github.com/eclipse-sumo/sumo
cd sumo
export SUMO_HOME="$PWD"
cmake -B build .
cmake --build build -j$(nproc)
```

Each of these steps is described in more detail and with possible
alternatives below.

## Installing required tools and libraries

- For the build infrastructure you will need cmake together with a moderately
  recent g++ (4.8 will do) or clang++ (or any other C++11 enabled compiler).
- The library Xerces-C is always needed. To use
  [sumo-gui](../sumo-gui.md) you also need Fox Toolkit in version
  1.6.x. It is highly recommended to also install Proj to have support
  for geo-conversion and referencing. Another common requirement is
  network import from shapefile (arcgis). This requires the GDAL
  library. To compile you will need the devel versions of all packages.
  For openSUSE this means installing libxerces-c-devel, libproj-devel,
  libgdal-devel, and fox16-devel. For ubuntu the call is above.
  The installation of swig, python3-dev and the jdk enables also the build of libsumo
  while eigen3 is necessary for the overheadwire model.
  There are some outdated [platform specific
  and manual build instructions for the
  libraries](Linux_Build_Libraries.md)
- Optionally you may want to add
 - ccache (to speed up builds)
 - ffmpeg-devel (for video output),
 - libOpenSceneGraph-devel (for the experimental 3D GUI),
 - gtest (for [unit testing](../Developer/Unit_Tests.md), do not use 1.13 or later)
 - gettext (for internationalization)
 - texttest, xvfb  and tkdiff (for the acceptance tests)
 - flake, astyle and autopep for style checking
 - see also further dependencies [for GUI testing](../Developer/GUI_Testing.md)

The package names above are for openSUSE, for ubuntu the call to get the most important optional libraries and tools is:

```
sudo apt-get install ccache libavformat-dev libswscale-dev libopenscenegraph-dev python3-pip python3-setuptools
sudo apt-get install libgtest-dev gettext tkdiff xvfb flake8 astyle python3-autopep8
pip3 install texttest
```

For the Python tools there are some requirements depending on which tools you want to use. If you want to install
everything using pip do `pip install -r tools/requirements.txt`. To install the most common dependencies with your
package manager on ubuntu do:

```
sudo apt-get install python3-pandas python3-rtree python3-pyproj
```

## Getting the source code

For the correct setting of SUMO_HOME you have to remember the correct
path, where you build your SUMO, the SUMO build path. This path is shown
with pwd at the end of getting the source code. If you want to develop
actively on sumo we strongly recommend to use the git repository. Please
build the tarball version if you need for some reason a specific version
of sumo.

### repository checkout (recommended)

The following commands should be issued:

```
git clone --recursive https://github.com/eclipse-sumo/sumo
cd sumo
git fetch origin refs/replace/*:refs/replace/*
pwd
```

The additional fetch of the replacements is necessary to get a full
local project history.

### release version or nightly tarball

Download
[sumo-src-{{Version}}.tar.gz](https://sumo.dlr.de/releases/{{Version}}/sumo-src-{{Version}}.tar.gz) or <https://sumo.dlr.de/daily/sumo-src-git.tar.gz>

```
tar xzf sumo-src-<version>.tar.gz
cd sumo-<version>/
pwd
```

## Definition of SUMO_HOME

Before compiling is advisable to
define the environment variable SUMO_HOME. SUMO_HOME must be set to
the SUMO build path from the previous step. Assuming that you placed
SUMO in the folder "*/home/<user\>/sumo-<version\>*", if you want to
define only for the current session, type in the console

```
export SUMO_HOME="/home/<user>/sumo-<version>"
```

If you want to define for all sessions (i.e. for every time that you run
your Linux distribution), go to your HOME folder, and find one of the
next three files (depending of your Linux distribution):
**.bash_profile**, **.bash_login** or **.profile** (Note that these
files can be hidden). Then edit the file, add the line from above at the
end and restart your session.

You can check that SUMO_HOME was successfully set if you type

```
echo $SUMO_HOME
```

and console shows "/home/<user\>/sumo-<version\>"

## Installing Python packages for the tools

Calling the tools from netedit requires a list of Python packages to generate templates during compilation.
Many of them might be available with the package manager of your distribution and most of the time we prefer
to use those. For ubuntu this currently means, you should first do

```
sudo apt-get install python3-pyproj python3-rtree python3-pandas python3-flake8 python3-autopep8 python3-scipy python3-pulp python3-ezdxf
```

and then install the remaining parts using pip:

```
pip install -r tools/requirements.txt
```

The pip installation will ensure that all libraries are there, so it is safe to skip the first `apt-get` step.
If you need information about the minimum required versions of the packages read them directly
from the [requirements.txt](https://github.com/eclipse-sumo/sumo/blob/main/tools/requirements.txt). Be aware that
the minimum versions in the requirements file just reflect our current test server setup, so you might also get away with earlier versions.

You might need to replace `pip` with `pip3` if you are using python3 on Linux.


## Building the SUMO binaries with cmake

To build with cmake version 3 or higher is required.

Create a build folder for cmake (in the SUMO root folder)
and configure SUMO with the full set of available options like GDAL and
OpenSceneGraph support (if the libraries are installed):

```
cmake -B build .
```

to build the debug version just use

```
cmake -D CMAKE_BUILD_TYPE=Debug -B build .
```

!!! note
    On some platforms the required cmake executable is called *cmake3*.

Other useful cmake configuration options:

- `-D PROFILING=ON` enable profiling instrumentation for gprof (gcc build only)
- `-D COVERAGE=ON` enable coverage instrumentation for lcov (gcc build only)
- `-D CHECK_OPTIONAL_LIBS=OFF` disable all optional libraries (only
  include EPL compatible licensed code)
- `-D CMAKE_BUILD_TYPE=RelWithDebInfo` enable debug symbols for
  debugging the release build or using a different profiler
- `-D PROJ_LIBRARY=` disable PROJ
- `-D FOX_CONFIG=` disable FOX toolkit (GUI and multithreading)
- `-D PYTHON_EXECUTABLE=/usr/bin/python3` select a different python version (also for libsumo / libtraci)
- `-D MVN_EXECUTABLE=` disable maven packaging (especially useful if you have no network connection)
- `-D ENABLE_CS_BINDINGS=ON` enable C# bindings when compiling libsumo / libtraci

After this is finished, run

```
cmake --build build -j $(nproc)
```

The `nproc` command gives you the number of logical cores on your
computer, so that make will start parallel build jobs which makes the
build a lot faster. If `nproc` is not available on your system, insert a
fixed number here or leave the option out. You may also try

```
cmake --build build -j $(grep -c ^processor /proc/cpuinfo)
```

## Building with clang

If you want to use a different compiler (just for the fun of it or
because it has additional features) you can enable it at configure time.
Our current clang configuration for additional static code checking
enables the following CXXFLAGS:

```
-stdlib=libstdc++ -fsanitize=undefined,address,integer,unsigned-integer-overflow -fno-omit-frame-pointer -fsanitize-blacklist=$SUMO_HOME/build_config/clang_sanitize_blacklist.txt
```

You may of course leave out all the sanitizer-checks you don't want but
the stdlib option has to be set. The blacklist filters out a known bug
in the cstdlib. For details see the clang documentation.

The current CMake configuration already contains this for the debug
build, so for building with CMake and clang just change to your build
dir and use

```
CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Debug --build build -j $(nproc)
```

The clang-debug-build will detect memory leaks (among other things)
If the errors are reported with cryptic hexadecimal numbers as

```
Indirect leak of 72 byte(s) in 1 object(s) allocated from:
    #0 0xa4ee2d  (.../sumo/bin/netconvertD+0xa4ee2d)
```

set the following environment variable to point to the llvm-symbolizer executable:
`export ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer` before running the executable.

## Installing the SUMO binaries

This (optional) step will copy the SUMO binaries to another path, so
that you can delete all source and intermediate files afterwards. If you
do not want (or need) to do that, you can simply skip this step and run
SUMO from the bin subfolder (bin/sumo-gui and bin/sumo).

If you want to install the SUMO binaries into your system, run
```
sudo cmake --install build
```

You have to adjust your SUMO_HOME variable to the install dir (usually
/usr/local/share/sumo)
```
export SUMO_HOME=/usr/local/share/sumo
```

## Uninstalling

CMake provides no `make uninstall` so if you ever want to uninstall, run
```
sudo xargs rm < install_manifest.txt
```
from the same folder you ran `make install`. This will leave some empty
directories, so if you want to remove them as well, double check that
$SUMO_HOME points to the right directory (see above) and run
```
sudo xargs rm -r $SUMO_HOME
```

## Building Python wheels for sumolib, traci and libsumo

If you want to distribute sumolib, traci and/or libsumo as wheels
you can build those wheels directly from the tools tree. Please be aware
that nightly builds of those packages are also available on https://test.pypi.org
```
pip install wheel build
cd tools
python build_config/version.py build_config/setup-sumolib.py ./setup.py
python -m build --wheel
python build_config/version.py build_config/setup-traci.py ./setup.py
python -m build --wheel
python build_config/version.py build_config/setup-libsumo.py ./setup.py
python -m build --wheel
```
You will need a recent version of pip (>=22) for this to work. If for some reason
you cannot update your pip you can also use the (discouraged!) method of calling
setup.py directly.
```
cd tools
python build_config/setup-sumolib.py bdist_wheel
```
Please note that you always need to be in the tools directory for this to work
and your wheels will be placed in tools/dist. Furthermore the traci and the sumolib wheel
are platform and Python version independent while libsumo depends on the exact
platform and Python you built it with.

## (Frequent) Rebuilds

If you did a repository clone you can simply update it by doing `git pull`
from inside the SUMO_HOME folder. Then change to the build directory and run
`make -j $(nproc)` again.

If your underlying system changed (updated libraries) or you experience other
build problems please try a clean build first by removing the build directory (or at
least the CMakeCache.txt) and running cmake and make again before reporting a bug.

If you find yourself building very often after minor changes, consider installing
ccache and run cmake again. It will be picked up automatically and can dramatically
improve build speed.

## How to build JuPedSim and then build SUMO with JuPedSim

In this section, you will learn how to build the latest version of the pedestrian simulator JuPedSim and how to compile SUMO with this latest version of JuPedSim. First of all, clone the JuPedSim repository:

``` bash
git clone https://github.com/PedestrianDynamics/jupedsim.git
```
Note that this will clone the full repository, including the latest version of JuPedSim. **We strongly recommend to build the latest release of JuPedSim (not the latest version), which is officially supported by SUMO.** You can consult the [JuPedSim build procedure](https://github.com/PedestrianDynamics/jupedsim#readme); hereafter we propose a similar procedure. First check which is the [latest release](https://github.com/PedestrianDynamics/jupedsim/releases) then in the cloned directory checkout to the latest release. For example, for JuPedSim release v1.0.5, you would need to type:

``` bash
cd jupedsim
git checkout v1.0.5
cd ..
```

Outside the repository directory, but at the same level, you will build and install in two directories `jupedsim-build` and `jupedsim-install`, which get created automatically by the following commands:

``` bash
cmake -B jupedsim-build -DCMAKE_INSTALL_PREFIX=jupedsim-install jupedsim
cmake --build jupedsim-build
cmake --install jupedsim-build
```

You can also change the configuration to Debug (with `-DCMAKE_BUILD_TYPE=Debug`) and also enable multithreading (with `-j4`) as usual with CMake. Now to integrate the latest version of JuPedSim into SUMO, you need to have GEOS on your computer, for instance by typing `sudo apt-get install libgeos-dev` in a console. Then, please follow the standard build procedure for MacOS: since the JuPedSim install folder is at the same level of SUMO, it will be found automatically. Alternatively, you can notify CMake where is JuPedSim installed by setting `JUPEDSIM_CUSTOMDIR` when calling CMake.

For further remarks on the use of JuPedSim inside SUMO, please consult [this page](../Simulation/Pedestrians.md#jupedsim).

## Troubleshooting

### Problems with the socket subsystem

Problem:

```
recv ./foreign/tcpip/libtcpip.a(socket.o) (symbol belongs to implicit dependency /usr/lib/libsocket.so.1)
```

Solution:
<https://lists.danga.com/pipermail/memcached/2005-September/001611.html>

### ld cannot find an existing library (Fedora-23)

Problem:

```
/usr/bin/ld: cannot find -lfreetype
ls -lah /usr/lib64/libfreetype*
 lrwxrwxrwx. 1 root root   21 Jul 28 15:54 /usr/lib64/libfreetype.so.6 -> libfreetype.so.6.12.0
 lrwxr-xr-x. 1 root root 689K Jul 28 15:54 /usr/lib64/libfreetype.so.6.12.0
```

Solution: Install the dev package; for fedora:

```
sudo yum install freetype-devel
```

For details see
[stackoverflow](https://stackoverflow.com/questions/335928/ld-cannot-find-an-existing-library)
discussion.

### Additional notes for Cygwin users

At the moment GUI building is still troublesome. It depends whether you
want to use the X-Server or native Windows GUI. We tried native Windows
GUI only and had to change the installed libFOX-1.4.la such that it
contains

```
dependency_libs=' -lgdi32 -lglaux -ldl -lcomctl32 -lwsock32 -lwinspool -lmpr
-lpthread -lpng /usr/lib/libtiff.la /usr/lib/libjpeg.la -lz -lbz2 -lopengl32 -lglu32'
```

Your mileage may vary.
