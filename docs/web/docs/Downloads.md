---
title: Downloads
permalink: /Downloads/
---

Here, you will find SUMO both as sources and as compiled binaries.

!!! note
    Windows users probably want to use [this installer](http://prdownloads.sourceforge.net/sumo/sumo-win64-1.2.0.msi?download), Linux users might want to look at the available [repositories](Downloads.md#repositories) or use the [source](http://prdownloads.sourceforge.net/sumo/sumo-src-1.2.0.tar.gz?download).

Please [contact](Contact.md) us if you have any problems. If you
want to report a bug, please [open a GitHub
issue](https://github.com/DLR-TS/sumo/issues/new). For further
information about the changes between releases see the
[ChangeLog](ChangeLog.md).

!!! note
    If you have problems running SUMO because of missing files/commands/dlls please consult [FAQ#Basic_Usage](FAQ.md#basic_usage)

## Note on Licensing

SUMO is licensed under the
[EPL-2.0](https://eclipse.org/legal/epl-v20.html) using only [open
source libraries](Libraries_Licenses.md).

The standard build only contains code and windows binaries with Eclipse
approved licenses (especially no GPL and LGPL code). If you need
features like shapefile import or the OpenSceneGraph 3D GUI download the
"extra" build.

## Packages

SUMO is available as different packages. The contents of each package is
listed in the table below.

|   | bin  | build  | src (source code)  | user docs  |  developer docs (doxygen) | data  | examples  | tutorials  | tests  | tools (except jars)  | jars  |
|---|------|--------|--------------------|------------|---------------------------|-------|-----------|------------|--------|----------------------|-------|
| sumo-src-*XXX*.tar.gz sumo-src-*XXX*.zip  |   | X | X |   |   | X | X | X |   | X |   |
|  sumo-win??-*XXX*.zip sumo-win??-*XXX*.msi | X |   |   | X |   | X | X | X |   | X | X |
| sumo-all-*XXX*.tar.gz sumo-all-*XXX*.zip  |   | X | X | X | X | X | X | X | X | X | X |
| rpm  | (X) |   |   | X |   | X | X | X |   | X |   |

### Repositories

In addition to the open build service, there are a debian and an ubuntu
launchpad project as well as an archlinux package:

- <https://build.opensuse.org/project/show?project=home%3Abehrisch>
- <http://anonscm.debian.org/gitweb/?p=debian-science/packages/sumo.git>
- <https://launchpad.net/~sumo>
- <https://aur.archlinux.org/packages/sumo/>

To add sumo to your ubuntu (11.04 and later) you will need to do:

```
sudo add-apt-repository ppa:sumo/stable
sudo apt-get update
sudo apt-get install sumo sumo-tools sumo-doc
```

This is still an experimental feature, feedback is welcome.

## SUMO - Latest Release (Version {{Version}})

**Release date: {{ReleaseDate}}**

### MS Windows binaries

Contains the binaries (32 or 64 bit), all dlls needed, the examples,
tools, and documentation in HTML format.

- Download 64 bit installer: [sumo-win64-{{Version}}.msi](https://prdownloads.sourceforge.net/sumo/sumo-win64-{{Version}}.msi?download)
- Download 64 bit zip: [sumo-win64-{{Version}}.zip](https://prdownloads.sourceforge.net/sumo/sumo-win64-{{Version}}.zip?download)
- Download 64 bit zip with all extras: [sumo-win64extra-{{Version}}.zip](https://prdownloads.sourceforge.net/sumo/sumo-win64extra-{{Version}}.zip?download)
- Download 32 bit installer: [sumo-win32-{{Version}}.msi](https://prdownloads.sourceforge.net/sumo/sumo-win32-{{Version}}.msi?download)
- Download 32 bit zip: [sumo-win32-{{Version}}.zip](https://prdownloads.sourceforge.net/sumo/sumo-win32-{{Version}}.zip?download)

### Linux binaries

They are created by the [open build
service](https://en.opensuse.org/Build_Service) If the repositories do
not contain the libraries (like proj and gdal) they are either part of
the distribution or you will need them from another repository (you may
try one of the build service repositories here too, e.g.
[Application:Geo](https://download.opensuse.org/repositories/Application:/Geo/)).
At the moment there is no documentation included in the packages. The
repositories include a nightly build as well (called sumo_nightly).

- [openSUSE Leap 42.1 repository](http://download.opensuse.org/repositories/home:/behrisch/openSUSE_42.1/)
- [openSUSE Leap 42.2 repository](http://download.opensuse.org/repositories/home:/behrisch/openSUSE_Leap_42.2/)
- [openSUSE Leap 42.3 repository](http://download.opensuse.org/repositories/home:/behrisch/openSUSE_Leap_42.3/)
- [openSUSE Leap 15.0 repository](http://download.opensuse.org/repositories/home:/behrisch/openSUSE_Leap_15.0/)
- [openSUSE Leap 15.1 repository](http://download.opensuse.org/repositories/home:/behrisch/openSUSE_Leap_15.1/)
- [openSUSE Tumbleweed repository](http://download.opensuse.org/repositories/home:/behrisch/openSUSE_Tumbleweed/)
- [Fedora 25 repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_25/)
- [Fedora 26 repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_26/)
- [Fedora 27 repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_27/)
- [Fedora 28 repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_28/)
- [Fedora 29 repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_29/)
- [Fedora 30 repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_30/)
- [Fedora Rawhide repository](http://download.opensuse.org/repositories/home:/behrisch/Fedora_Rawhide/)

There are [more Linux RPM
repositories](https://build.opensuse.org/repositories/home:behrisch) but
in a less well maintained state (CentOS and Scientific missing gdal
support for instance) because the opensuse build service does not
provide the necessary packages any longer. Ubuntu and Debian users
please see above for repository information.

### macOS binaries

"Bottles" are available for installing with
[Homebrew](https://brew.sh/). They are built for the two most recent
major macOS versions (currently High Sierra and Mojave) and are built
from source with minimal requirements (fox, proj, xerces-c). If you need
optional libraries, you can specify these on the brew command line and
brew will compile sumo from source. For details, see the [Formula's
README](https://github.com/DLR-TS/homebrew-sumo/blob/master/README.md).

### Sources

Includes sources, examples, and CMake-files for creating Visual Studio
solutions or Linux Makefiles. Does not contain tests. Download as:

- [sumo-src-{{Version}}.tar.gz](https://prdownloads.sourceforge.net/sumo/sumo-src-{{Version}}.tar.gz?download)
- [sumo-src-{{Version}}.zip](https://prdownloads.sourceforge.net/sumo/sumo-src-{{Version}}.zip?download)

### All-inclusive-tarball

Includes sources, tests and docs but no binaries. Download as:

- [sumo-all-{{Version}}.tar.gz](https://prdownloads.sourceforge.net/sumo/sumo-all-{{Version}}.tar.gz?download)

## SUMO - Latest Development Version

SUMO is under active development. You can find a continuously updated
list of bug-fixes and enhancements at our
[ChangeLog](ChangeLog.md). To make use of the latest features
[(and to give us pre-release feedback)](Contact.md) we encourage
you to use the latest version from our [code repository](https://github.com/eclipse/sumo/).

### Nightly Snapshots

The code within the repository is [compiled each
night](Developer/Nightly_Build.md). The following resulting
packages can be obtained:

- <https://sumo.dlr.de/daily/sumo-src-git.tar.gz> (sources)
- <https://sumo.dlr.de/daily/sumo-src-git.zip> (sources)
- <https://sumo.dlr.de/daily/sumo-msvc12Win32-git.zip> (windows,
    32bit)
- <https://sumo.dlr.de/daily/sumo-msvc12x64-git.zip> (windows, 64bit)
- <https://sumo.dlr.de/daily/sumo-all-git.tar.gz> (sources,
    documentation and tests)
- <https://sumo.dlr.de/daily/sumo-game-msvc12Win32-git.zip> (windows
    32bit binaries of the sumo game)
- <https://sumo.dlr.de/daily/sumo-game-msvc12x64-git.zip> (windows
    64bit binaries of the sumo game)
- <https://sumo.dlr.de/daily/sumo-msvc12Win32-git.msi> (windows
    installer, 32bit)
- <https://sumo.dlr.de/daily/sumo-msvc12x64-git.msi> (windows
    installer, 64bit)
- <https://sumo.dlr.de/daily/sumo-msvc12extrax64-git.zip> (windows
    64bit with GDAL, FFmpeg, OSG, GL2PS, SWIG)

The Linux repositories specified above contain a nightly build as well.

[The corresponding documentation](https://sumo.dlr.de/daily/userdoc) is
also visible live including [Doxygen
docs](https://sumo.dlr.de/daily/doxygen). Additional artifacts such as
[tests results](https://sumo.dlr.de/daily) and [code coverage
analysis](https://sumo.dlr.de/daily/lcov/html/) are generated every
night.

!!! caution
    The available windows binary packages may lag behind the [latest Git revision](https://github.com/eclipse/sumo/commits/master) due to being compiled only once per day (around midnight, Berlin time).

### Nightly Snapshots alternative download server

All nightly builds are also available at the following alternative
locations

- <http://sumo.sourceforge.net/daily/sumo-src-git.tar.gz> (sources)
- <http://sumo.sourceforge.net/daily/sumo-src-git.zip> (sources)
- <http://sumo.sourceforge.net/daily/sumo-msvc12Win32-git.zip>
    (windows, 32bit)
- <http://sumo.sourceforge.net/daily/sumo-msvc12x64-git.zip> (windows,
    64bit)
- <http://sumo.sourceforge.net/daily/sumo-game-msvc12Win32-git.zip>
    (windows 32bit binaries of the sumo game)
- <http://sumo.sourceforge.net/daily/sumo-game-msvc12x64-git.zip>
    (windows 64bit binaries of the sumo game)
- <http://sumo.sourceforge.net/daily/sumo-msvc12Win32-git.msi>
    (windows installer, 32bit)
- <http://sumo.sourceforge.net/daily/sumo-msvc12x64-git.msi> (windows
    installer, 64bit)
- <http://sumo.sourceforge.net/daily/sumo-win64extra-git.zip> (windows 64bit with GDAL, FFmpeg, OSG, GL2PS, SWIG)

### Direct repository access

You can get very latest sources directly from our Git repository, see
[the FAQ on repository access](FAQ.md#how_do_i_access_the_code_repository).
Normally, they should compile and complete our test suite successfully.
To assess the current state of the build, you may take a look at the
[nightly test statistics](https://sumo.dlr.de/daily/).

## SUMO - older releases

Older releases can be obtained via the [sourceforge download
portal](https://sourceforge.net/projects/sumo/files/sumo/)

## Dependencies for developers

For the Windows platform you can retrieve all dependencies by cloning
this repository <https://github.com/DLR-TS/SUMOLibraries>, if you want
to develop with Visual Studio. If you just want to run SUMO, use the
binary downloads above which already contain the runtime dependencies.