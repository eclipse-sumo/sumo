---
title: Libraries Licenses
---

This page lists all licenses of code SUMO depends on directly for
compiling. There may be secondary references (like the C++ standard
library or a particular OpenGL implementation) which are not mentioned
here except for the cases where we deliver binaries for that code.

## Code in the repository

All the code which can be found under <SUMO\>/src and <SUMO\>/tools except
for <SUMO\>/src/foreign and <SUMO\>/tools/contributed is since 03.10.2017
(revision 26300) licensed under [EPL
V2](https://eclipse.org/legal/epl-v20.html). All earlier versions were
licensed under [GPL V3+](https://gnu.org/licenses/gpl.html). The code in
<SUMO\>/src/foreign and <SUMO\>/tools/contributed should have separate
license files but we strive to list them here as well:

### [tcpip](https://github.com/itm/shawn/tree/master/src/apps/tcpip) - TCP/IP Socket Class to communicate with other programs

- Version: <https://github.com/itm/shawn/tree/master/src/apps/tcpip>
  from 2012-09-28
- Modifications: fixed invalid buffer length calculation, fixed
  compiler warnings, removed "using namespace std"
- License: [based on BSD
  License](https://github.com/itm/shawn/blob/master/shawn-licence.txt)

### [Font Stash](https://github.com/memononen/fontstash) - Text rendering in openGL

- Version: <https://github.com/memononen/fontstash> branch master from
  2017-09-23
- Modifications: only included necessary files fontstash.h,
  glfontstash.h, stb_truetype.h
- License: [zlib
  License](https://github.com/memononen/fontstash/blob/master/LICENSE.txt),
  includes std_truetype which is in the public domain

### [RTree](https://www.superliminal.com/sources/RTreeTemplate.zip) - Improved tiling for the drawing area

- Version: unknown (2003)
- Modifications: syntactic sugar for gcc compilation; different
  footprint for "Search" due to a different callback, tests removed
  because they did not compile
- License: public domain (see
  [README.TXT](https://github.com/eclipse-sumo/sumo/blob/main/src/foreign/rtree/README.TXT))

### [odrSpiral](https://github.com/DLR-TS/odrSpiral) - Approximating curves

- Used version: <https://github.com/DLR-TS/odrSpiral> branch master
  from 2017-05-03
- Modifications: includes and constants;
- License: Apache License 2.0, see [license
  file](https://github.com/eclipse-sumo/sumo/blob/main/src/foreign/eulerspiral/LICENSE.TXT)

### [zstr](https://github.com/mateidavid/zstr) - A header only C++ ZLib wrapper

- Version: 1.0.6
- Modifications: only needed header files, includes fixed
- License: MIT, see [license
  file](https://github.com/eclipse-sumo/sumo/blob/main/src/foreign/zstr/LICENSE)

# Referenced Libraries

## Mandatory and highly recommended

### [Microsoft Visual C++ Redistributable](https://www.google.com/url?q=https://support.microsoft.com/en-us/kb/2977003) - Visual C++ Runtime Components (Windows only)

- [Microsoft Software
  License](https://www.visualstudio.com/en-us/mt171551.aspx)

### [Xerces-C++](https://xerces.apache.org/xerces-c/) - XML Parser

- [Apache Software License
  version 2.0](https://www.apache.org/licenses/LICENSE-2.0)

### [FOX toolkit](https://fox-toolkit.org/) - GUI Widget Toolkit

can be omitted, but [sumo-gui](sumo-gui.md),
[netedit](Netedit/index.md) and multithreading won't work

- [GNU Lesser General Public
  License](https://www.gnu.org/copyleft/lesser.html) with static
  relinking exemption
  - zlib - [self made license, free also for commercial use and
    binary distribution](https://www.zlib.net/zlib_license.html)
  - libpng - [self made license, comparable to
    zlib](http://www.libpng.org/pub/png/src/libpng-LICENSE.txt)
  - libjpeg - [self made license, free also for commercial use and
    binary
    distribution](https://github.com/winlibs/libjpeg/blob/master/README)

### [PROJ.4](https://proj.org/) - Cartographic Projections Library

can be omitted, but import of geo referenced networks like OpenStreetMap
won't work

- [MIT derived license](https://proj.org/about.html#license)

## Optional

### [GDAL](https://www.gdal.org/) - Geospatial Data Abstraction Library

- [​X11/​MIT
  License](https://svn.osgeo.org/gdal/trunk/gdal/LICENSE.TXT)

### [gl2ps](https://www.geuz.org/gl2ps/) - Printing screenshots as ps or pdf files

- Used version: 1.3.9
- License: [GNU Library General Public
  License](https://www.geuz.org/gl2ps/COPYING.LGPL) or [GL2PS
  License](https://www.geuz.org/gl2ps/COPYING.GL2PS)

### [Open Scene Graph](https://www.openscenegraph.org/) - 3D Graphics Toolkit

- [OpenSceneGraph Public
  License](https://openscenegraph.github.io/openscenegraph.io/about/license.html)

### [FFmpeg](https://ffmpeg.org/) - Multimedia Library

- [GNU Lesser General Public
  License](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)

### [Google Test](https://github.com/google/googletest) - C++ Unit Test Framework

- [BSD License](https://github.com/google/googletest/blob/main/LICENSE)

### [GNU gettext](https://www.gnu.org/software/gettext/) - Multiple Language support

- [GNU Lesser General Public
  License](https://www.gnu.org/software/gettext/manual/html_node/GNU-LGPL.html) for the libintl part (the distributed runtime)

Please note that the tools needed for processing the translation files (so for compiling SUMO)
are distributed under GPL and are not part of the SUMO download.

### [{fmt}](https://fmt.dev) - Formatting library

- [​MIT License](https://github.com/fmtlib/fmt/blob/master/LICENSE)


## Data and Documentation

Unless otherwise noted all documentation (including the files in this directory and below) and data
files are licensed under the [Eclipse Public License v2](https://www.eclipse.org/legal/epl-v20.html) just as the rest of SUMO.

### 3D models

All models in the data/3D directory originate from
<https://opengameart.org/content/vehicles-assets-pt1> and are in the
public domain

<p style="border: 1px solid #909090; padding: 1px 4px"><img src="images/CC-CC0-small.png" alt="CC0"> This work is licensed under a <a href="https://creativecommons.org/publicdomain/zero/1.0/">Creative Commons CC0 1.0 Universal license</a>. To the extent possible under law, the person who associated CC0 with this work has waived all copyright and related or neighboring rights to this work.</p>

### OpenStreetMap data

All test inputs which build upon OpenStreetMap data (mainly the osm.xml
files in [{{SUMO}}/tests/netconvert/import]({{Source}}tests/netconvert/import)) are under the [Open Database
License](https://opendatacommons.org/licenses/odbl/).

### Fonts

Currently the [Google Roboto](https://fonts.google.com/specimen/Roboto)
font is included which is available under an [Apache
License](https://www.apache.org/licenses/LICENSE-2.0).
