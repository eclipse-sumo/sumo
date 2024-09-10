---
title: Windows Libraries
---

!!! note
    Most of the time it is not necessary to build the libraries yourself. You can just use the versions available at <https://github.com/DLR-TS/SUMOLibraries> as described in the [Windows build instructions](../Installing/Windows_Build.md#libraries).

## Individual packages

### Xerces-C

SUMO uses Xerces-C 3.1.x or 3.2.x by default. Please be
aware that Xerces-C 3.1.4 can cause problems when using unicode
characters in your networks (especially importing chinese street names
from OpenStreetMap).

Apache does not provide binary distributions any longer, but you can
try our own build of [Xerces-C 3.2.4 in the SUMOLibraries](https://github.com/DLR-TS/SUMOLibraries).
Please note that setting environment variables to find a custom Xerces build does not work any longer.

### Fox

- If you do not need a GUI, you can skip this section.
- SUMO uses Fox 1.6 and will *not* work with Fox 1.7.
- Usually you do not need to build Fox yourself you can [use the
readily compiled
binaries](https://github.com/DLR-TS/SUMOLibraries).
- If you insist, please [download fresh
sources](https://www.fox-toolkit.org/download.html) and
replace the ones in the SUMOLibraries (make sure not to delete the project file mentioned below).
- Go to the Fox directory and open the VC project e.g.
D:\\SUMOLibraries\\fox-1.6.58\\windows\\vcpp\\x64.sln.
- You might get approximately 240 warnings and one error, which can
probably be ignored as long as you can find the resulting DLLs in
the lib dir.
- Errors on not finding windows.h mean the SDK was not installed
properly.

#### PNG support in Fox

- Warning: This is a little advanced, we were successful with libpng
1.5.2 and 1.6.20 together with zlib 1.2.5 and 1.2.8
- download zlib sources and libpng sources from
[\[1\]](http://www.libpng.org/pub/png/libpng.html) and
[\[2\]](https://www.zlib.net/)
- unzip all zips
- there is no need to build zlib
- build pnglib by first adapting the path in
<libpng\>\\projects\\vstudio\\zlib.props (if necessary) and then use
the vstudio.sln from the same dir to build the libpng project
- you will get a png lib and a dll in
<libpng\>\\projects\\vstudio\\Debug\\
- if you want to be really sure, you can build the pngtest project as
well and check the output
- you can build / add further configurations / platforms if needed
- Open the Fox project solution and edit properties of foxdll
  - in the C++/General section add the <libpng\> dir to the include
    path
  - in the C++/Preprocessor section add the HAVE_PNG_H define
  - in the Linker/Input section add the .lib file to the input
- recompile fox and put the resulting dll together with libpng dll in
your path

#### JPEG support in Fox

- Warning: This is a little advanced, we were successful with jpeg 8d,
and 9b the jpeg 8c which can be found at the fox-toolkit site was
broken
- download libjpeg sources from [\[4\]](https://www.ijg.org/) and unzip it
- download win32.mak and place it in the jpeg dir
- open a visual studio command prompt, cd to the jpeg dir and execute

```
nmake /f makefile.vc setup-v10
nmake /f makefile.vc
```

- you will get a libjpeg lib (and no dll\!) in the jpeg dir
- if you need different configurations / platforms you can also open
the solution file jpeg.sln and build jpeg from the GUI

<!-- end list -->

- Open the Fox project solution and edit properties of foxdll
  - in the C++/General section add the jpeg dir to the include path
  - in the C++/Preprocessor section add the HAVE_JPEG_H define
  - in the Linker/Input section add the .lib file to the input
- recompile foxdll (see above) and put the resulting dll (which has
jpeg support linked statically inside) in your path

### PROJ and GDAL

If you do not need transformation of geocoordinates you can disable PROJ
and GDAL in the CMakeCache.txt
(set all lines referring to PROJ or GDAL to empty) and
skip this section. Otherwise you have the choice between downloading the
binary packages [from the SUMOLibraries](https://github.com/DLR-TS/SUMOLibraries)
or compiling for yourself (for the
adventurous). Building from source should only be necessary if you want
a 32bit build or some outdated version.

#### Building from source

If you want to build PROJ and GDAL yourself for some reason, please
follow the relevant [build instructions for
PROJ](https://proj.org/install.html) and [for
GDAL](https://gdal.org/download.html#build-requirements). You then
should replace the version in the SUMOLibraries with your build.

### FFMPEG

This is only needed, if you want to save videos directly from the
sumo-gui and can for some reason not use the version provided in the
[SUMOLibraries](https://github.com/DLR-TS/SUMOLibraries).

- You just need to download [the binary ffmpeg
  distributions](https://ffmpeg.org/download.html) in the Shared and
  the Dev flavor. Also be sure to download all the platforms (32
  and/or 64 bit) you need.
- Unzip the Dev version and overwrite the version in your SUMOLibraries with it.
- Unzip the Share version and copy the dll-files from the bin
  directory into SUMOLibraries as well.

### OpenSceneGraph

This is only relevant for the 3D GUI. You will need to build at least
OSG 3.4.0 because earlier versions do not work with Visual Studio 2012.
Unfortunately there are no binaries readily available. If you want to
use models which are not in <SUMO\>/data/3D, you can add directories
containing them to the OSG_FILE_PATH environment variable. the build
will look for libraries and includes in the SUMOLibraries.
