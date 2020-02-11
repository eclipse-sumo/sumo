---
title: Downloads
permalink: /Downloads/
---

Here, you will find SUMO both as sources and as compiled binaries.

!!! note
    Windows users probably want to use [this installer](https://sumo.dlr.de/releases/{{Version}}/sumo-win64-{{Version}}.msi), Linux users might want to look at the available [repositories](Downloads.md#repositories) or use the [source](https://sumo.dlr.de/releases/{{Version}}/sumo-src-{{Version}}.tar.gz).

Please [contact](Contact.md) us if you have any problems. If you
want to report a bug, please [open a GitHub
issue](https://github.com/eclipse/sumo/issues/new). For further
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
| sumo-src-*XXX*.tar.gz<br>sumo-src-*XXX*.zip  |   | &#10004; | &#10004; |   |   | &#10004; | &#10004; | &#10004; |   | &#10004; |   |
|  sumo-win??-*XXX*.zip<br>sumo-win??-*XXX*.msi | &#10004; |   |   | &#10004; |   | &#10004; | &#10004; | &#10004; |   | &#10004; | &#10004; |
| sumo-all-*XXX*.tar.gz<br>sumo-all-*XXX*.zip  |   | &#10004; | &#10004; | &#10004; |   | &#10004; | &#10004; | &#10004; | &#10004; | &#10004; | &#10004; |
| rpm  | (&#10004;) |   |   | &#10004; |   | &#10004; | &#10004; | &#10004; |   | &#10004; |   |

### Repositories

In addition to the open build service, there are a debian and an ubuntu
launchpad project as well as an archlinux package:

- <https://build.opensuse.org/project/show/home:behrisch>
- <https://salsa.debian.org/science-team/sumo.git>
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

<ul>
<li>Download 64 bit installer: <a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-win64-{{Version}}.msi">sumo-win64-{{Version}}.msi </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win64-{{Version}}.msi","r");?></span></li>
<li>Download 64 bit zip: <a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-win64-{{Version}}.zip">sumo-win64-{{Version}}.zip </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win64-{{Version}}.zip","r");?></span></li>
<li>Download 64 bit zip with all extras: <a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-win64extra-{{Version}}.zip">sumo-win64extra-{{Version}}.zip </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win64extra-{{Version}}.zip","r");?></span></li>
<li>Download 32 bit installer: <a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-win32-{{Version}}.msi">sumo-win32-{{Version}}.msi </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win32-{{Version}}.msi","r");?></span></li>
<li>Download 32 bit zip: <a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-win32-{{Version}}.zip">sumo-win32-{{Version}}.zip </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win32-{{Version}}.zip","r");?></span></li>
</ul>

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

<ul>
<li><a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-src-{{Version}}.tar.gz">sumo-src-{{Version}}.tar.gz </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-src-{{Version}}.tar.gz","r");?></span></li>
<li><a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-src-{{Version}}.zip">sumo-src-{{Version}}.zip </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-src-{{Version}}.zip","r");?></span></li>
</ul>

### All-inclusive-tarball

Includes sources, tests and docs but no binaries. Download as:

<ul>
<li><a class="no-arrow-link" href="https://sumo.dlr.de/releases/{{Version}}/sumo-all-{{Version}}.tar.gz">sumo-all-{{Version}}.tar.gz </a><span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-all-{{Version}}.tar.gz","r");?></span></li>
</ul>

## SUMO - Latest Development Version

SUMO is under active development. You can find a continuously updated
list of bug-fixes and enhancements at our
[ChangeLog](ChangeLog.md). To make use of the latest features
[(and to give us pre-release feedback)](Contact.md) we encourage
you to use the latest version from our [code repository](https://github.com/eclipse/sumo/).

### Nightly Snapshots

<div><span class="badge badge-pill badge-dark"><?php getNightlyFreshness("sumo-win64-git.zip");?></span></div>

The code within the repository is [compiled each
night](Developer/Nightly_Build.md). The following resulting
packages can be obtained:

<ul>
<li>Sources: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-src-git.tar.gz">https://sumo.dlr.de/daily/sumo-src-git.tar.gz </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-src-git.tar.gz","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-src-git.tar.gz","d");?></span></li>
<li>Sources: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-src-git.zip">https://sumo.dlr.de/daily/sumo-src-git.zip </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-src-git.zip","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-src-git.zip","d");?></span></li>
<li>Sources, documentation and tests: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-all-git.tar.gz">https://sumo.dlr.de/daily/sumo-all-git.tar.gz </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-all-git.tar.gz","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-all-git.tar.gz","d");?></span></li>
<li>Windows, 32bit: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-win32-git.zip">https://sumo.dlr.de/daily/sumo-win32-git.zip </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-win32-git.zip","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win32-git.zip","d");?></span></li>
<li>Windows, 64bit: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-win64-git.zip">https://sumo.dlr.de/daily/sumo-win64-git.zip </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-win64-git.zip","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win64-git.zip","d");?></span></li>
<li>Windows 64bit binaries of the SUMO game: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-game-win64-git.zip">https://sumo.dlr.de/daily/sumo-game-win64-git.zip </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-game-win64-git.zip","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-game-win64-git.zip","d");?></span></li>
<li>Windows installer, 32bit: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-win32-git.msi">https://sumo.dlr.de/daily/sumo-win32-git.msi </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-win32-git.msi","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win32-git.msi","d");?></span></li>
<li>Windows installer, 64bit: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-win64-git.msi">https://sumo.dlr.de/daily/sumo-win64-git.msi </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-win64-git.msi","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win64-git.msi","d");?></span></li>
<li>Windows 64bit with GDAL, FFmpeg, OSG, GL2PS, SWIG: <a class="no-arrow-link" href="https://sumo.dlr.de/daily/sumo-win64extra-git.zip">https://sumo.dlr.de/daily/sumo-win64extra-git.zip </a><span class="badge badge-pill badge-light"><?php getFileDate("sumo-win64extra-git.zip","d");?></span> <span class="badge badge-pill badge-secondary"><?php getFileSize("sumo-win64extra-git.zip","d");?></span></li>
</ul>

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
- <http://sumo.sourceforge.net/daily/sumo-win32-git.zip> (windows, 32bit)
- <http://sumo.sourceforge.net/daily/sumo-win64-git.zip> (windows, 64bit)
- <http://sumo.sourceforge.net/daily/sumo-game-win64-git.zip> (windows 64bit binaries of the sumo game)
- <http://sumo.sourceforge.net/daily/sumo-win32-git.msi> (windows installer, 32bit)
- <http://sumo.sourceforge.net/daily/sumo-win64-git.msi> (windows installer, 64bit)

### Direct repository access

You can get very latest sources directly from our Git repository, see
[the FAQ on repository access](FAQ.md#how_do_i_access_the_code_repository).
Normally, they should compile and complete our test suite successfully.
To assess the current state of the build, you may take a look at the
[nightly test statistics](https://sumo.dlr.de/daily/).

## SUMO - Alternative download and older releases

All releases can also be obtained via the [sourceforge download
portal](https://sourceforge.net/projects/sumo/files/sumo/)

## Dependencies for developers

For the Windows platform you can retrieve all dependencies by cloning
this repository <https://github.com/DLR-TS/SUMOLibraries>, if you want
to develop with Visual Studio. If you just want to run SUMO, use the
binary downloads above which already contain the runtime dependencies.

<?php
function getFileDate($fname, $type){
    switch($type){
    case "r":
    $file = "/releases/{{Version}}/" . $fname;
    break;
    case "d":
    $file = "/daily/" . $fname;
    break;
}
$file = $_SERVER['DOCUMENT_ROOT']. $file;
if(file_exists($file)){
    echo date ("F d Y H:i:s", filemtime($file)) . " UTC";
}}
function getFileSize($fname, $type){
switch($type){
    case "r":
    $file = "/releases/{{Version}}/" . $fname;
    break;
    case "d":
    $file = "/daily/" . $fname;
    break;
}
$file = $_SERVER['DOCUMENT_ROOT']. $file;
if(file_exists($file)){
echo round(((filesize($file))/1048576),1) . " MB";
}}
function getNightlyFreshness($fname){
$zip = new ZipArchive;
$zip->open($_SERVER['DOCUMENT_ROOT']. "/daily/" . $fname);
$freshnessIs = str_replace("\"","",str_replace("#define VERSION_STRING ","",$zip->getFromName('sumo-git/include/version.h')));
echo $freshnessIs;
$zip->close();
}
?>
