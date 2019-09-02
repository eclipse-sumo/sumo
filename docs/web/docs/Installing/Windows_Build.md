---
title: Installing/Windows Build
permalink: /Installing/Windows_Build/
---

This document describes how to build SUMO under MS-Windows using only
freely available (this does **not** mean "open source") tools.
Instructions on how to build SUMO on Windows using an Open Source
toolchain are included in our [building on
Linux](../Installing/Linux_Build.md) pages. Please note that you
may also [download pre-built Windows binaries](../Downloads.md).

## Short overview

- Download [Visual C++ Community
  Edition](https://www.visualstudio.com/vs/community/). SUMO is only
  compatible with Visual Studio 2013 or higher. If you are using
  Windows 8 or later be sure to download Visual Studio for Windows
  *Desktop*. Please install all the available Service Packs for Visual
  Studio as well. Note that with Visual Studio Community 2017 SUMO
  only can be compiled **in Release Mode**.
- [Download Python for Windows](http://www.python.org/download/) and
  install it. Our most preferred version is Python 2.7.x for the 32
  bit platform, but you may try Python 3 and / or 64bit as well.
  Please be aware that the test environment needs Python 2.7 32bit

  !!! note
      If you have Visual Studio 2017 you may skip the following steps and jump to [Installing/Windows_Build#Doing_everything_inside_Visual_Studio](#doing_everything_inside_visual_studio_2017_or_later)

- Download and install [CMake](https://cmake.org/download/)
- Download and install [Git](https://git-scm.com/download/win)
- Clone the sumo repository using ` git clone https://github.com/eclipse/sumo` or download and extract a source
  package, see [Downloads](../Downloads.md)
  - set SUMO_HOME to the cloned directory
- Install the [\#Libraries](#libraries) using ` git clone https://github.com/DLR-TS/SUMOLibraries` and set the
  SUMO_LIBRARIES environment variable
- Run the CMake-GUI and set the source path and define a build
  directory, for more details see
  [Installing/Windows_CMake](../Installing/Windows_CMake.md)
- Press Configure, select the Visual Studio 2017 Win64 Generator, then
  press Generate and Open Project
- Compile SUMO in the opened Visual Studio

## Doing everything inside Visual Studio 2017 (or later)

Visual Studio 2017 already brings git and CMake support, so you can

- Team-\>Manage Connections and then choose Clone under Local Git
repositories (see here for an [Introduction to Git usage with Visual
Studio](https://docs.microsoft.com/en-us/azure/devops/repos/git/gitquickstart)
including updating and comitting)
  - once for <https://github.com/eclipse/sumo>
  - then for <https://github.com/DLR-TS/SUMOLibraries>
- Visual Studio will try to generate the solutions using CMake
automatically
  - will fail because the path to SUMOLibraries is not set (it does
    not find Xerces)
  - Choose to edit CMakeCache.txt from the CMake menu and set the
    path (e.g.
    SUMO_LIBRARIES:PATH=C:/Users/testus/source/repos/SUMOLibraries)
  - Select Generate again from CMake menu
- Select CMake-\>Build All

## Libraries

We provide a central location for getting all dependent libraries at
<https://github.com/DLR-TS/SUMOLibraries>. The easiest way is to clone
this repository and define an environment variable `SUMO_LIBRARIES`
pointing to the resulting directory. They are built with Visual Studio
2017, but may be used with earlier and later versions as well. You may
need to install the Visual C++ 2017 Runtime Distributable for running
SUMO then (tested with Visual Studio 2013). For details on building your
own and also on how to use different versions and additional libraries
see
[Installing/Windows_Libraries](../Installing/Windows_Libraries.md).
Make sure that the subdirectories `fox-1.6.54/lib`, `proj_gdal-1911/bin`
and `xerces-c-3.2.0/bin` are in PATH. Note: for 64 bits compilations
folders are `fox-1.6.54_64/lib`, `proj_gdal-1911_64/bin` and
`xerces-c-3.2.0_64/bin`. You can add both to the path but always add the
64 bit version first.

## Tests

- If you plan to extend SUMO yourself, or just want to know whether
  everything compiled OK, it is strongly recommended to have a look at
  [Developer/Tests](../Developer/Tests.md). This tool makes it
  easier to check whether some existing functionality was broken by
  the extensions.

## Troubleshooting

- Note on installation paths: MSVC seems to have difficulties with
  include and library paths containing spaces (for instance
  `C:\Program Files`). Thus try to avoid installing SUMO or any of the
  libraries in such paths.

### Linker reports something similar to "LINK : fatal error LNK1104: cannot open file 'C:\\Program.obj'"

You probably have installed a library to a path containing white spaces
in its name. In such a case, the according environment variable should
be embedded in quotes (").

Example: set FOX="D:\\my libs\\fox-1.6.36".

### Failure on pre-build event (missing version.h or \*typemap.h)

If Visual Studio reports a failed pre-build event you can safely ignore
this, unless you are building from the [source code
repository](../FAQ.md#how_do_i_access_the_code_repository). In
this case you should probably install Python. Even if python is
installed the file associations may be broken which causes the
generation of src/version.h via tools/build/version.py to fail. Either
repair your file associations or undefine HAVE_VERSION_H in
src/windows_config.h.

If you did install Python correctly, double check that it passes
[command line
arguments](http://stackoverflow.com/questions/2640971/windows-is-not-passing-command-line-arguments-to-python-programs-executed-from-t).
For a quick fix, you can also execute the following commands manually:

```
tools\build\version.py
tools\build\typemap.py
```

### Execution cannot proceed because MSVCR120.dll/MSVCR140.dll was not found

Install Microsoft [Visual C++ Redistributable Packages for Visual
Studio 2012](https://www.microsoft.com/en-US/download/details.aspx?id=30679)
(for MSVCR120.dll) or [Microsoft Visual C++ Redistributable Packages for
Visual Studio 2015](https://www.visualstudio.com/downloads/) (for
MSVCR140.dll). You can check if all dependences are correct using
[Dependencies](https://lucasg.github.io/Dependencies/)

![](../images/Dependencies.png)

### In debug mode, execution cannot proceed because MSVCR120D.dll/MSVCR140D.dll was not found

Your version of Visual Studio doesn't support Debugging, only can be
compiled in release mode.

## Available configurations

The release build is used for the distribution of sumo. The Debug build
allows all debugging features. Keep in mind that
[Texttest](../Developer/Tests.md) usually picks up the release
build.

- Release: All optimizations, assertions disabled, no debugging
  symbols, links against external release libs
- Debug: No optimizations, assertions enabled, debugging symbols
  included, links against external debug libs

Left clicking over Solution/Properties/Configuration Manager allow to
change between configurations:

![](../images/SwichDebugRelease.png)

To switch to a different platform (e.g. 32bit instead of 64 bit) please
run cmake again with a different generator.

Naming Conventions: 64bit executables have the same name as their 32bit
counterpart. The Debug build additionally carries the suffix 'D'.