---
title: Installing
permalink: /Installing/
---

# Windows

There are four different binary packages for Windows depending on the
platform (32 vs. 64 bit) you have and what you want to do with SUMO. If
you want to install it locally and have administrator rights on your
machine you should download and execute one of the installers
(preferably 64 bit). If you need a "portable" version or do not have
admin rights, use the correct zip, extract it into a desired folder
using [7Zip](https://7-zip.de/),
[Winzip](https://www.winzip.com/win/de/prod_down.html), or a similar tool. Every
package contains the binaries, all dlls needed, the examples, tools, and
documentation in HTML format.

- Download 64 bit installer: [sumo-win64-{{Version}}.msi](http://prdownloads.sourceforge.net/sumo/sumo-win64-{{Version}}.msi?download)
- Download 64 bit zip: [sumo-win64-{{Version}}.zip](http://prdownloads.sourceforge.net/sumo/sumo-win64-{{Version}}.zip?download)
- Download 32 bit installer: [sumo-win32-{{Version}}.msi](http://prdownloads.sourceforge.net/sumo/sumo-win32-{{Version}}.msi?download)
- Download 32 bit zip: [sumo-win32-{{Version}}.zip](http://prdownloads.sourceforge.net/sumo/sumo-win32-{{Version}}.zip?download)

Within the installation folder, you will find a folder named "**bin**".
Here, you can find the executables (programs). You may double click on
[SUMO-GUI](SUMO-GUI.md) and take a look at the examples located
in **docs/examples**. All other applications
([DUAROUTER](DUAROUTER.md), [DFROUTER](DFROUTER.md),
etc.) have to be run from the command line. To facilitate this there is
also a start-commandline.bat which sets up the whole environment for
you. If you feel unsure about the command line, please read
[Basics/Basic_Computer_Skills\#Running_Programs_from_the_Command_Line](Basics/Basic_Computer_Skills.md#running_programs_from_the_command_line).

If you want a bleeding edge nightly build or need tests or source files,
you can download them from the [Download](Downloads.md) page.

For building SUMO from source see [building SUMO under Windows](Installing/Windows_Build.md).

# Linux

If you run debian or ubuntu, SUMO is part of the regular distribution
and can be installed like this:

```
sudo apt-get install sumo sumo-tools sumo-doc
```

If you need a more up-to-date ubuntu version, it may be found in a
separate ppa, which is added like this:

```
sudo add-apt-repository ppa:sumo/stable
sudo apt-get update
```

and then again

```
sudo apt-get install sumo sumo-tools sumo-doc
```

Precompiled binaries for different distributions like openSUSE and
Fedora can be found at these [repositories for binary Linux versions](http://download.opensuse.org/repositories/home:/behrisch/).
These repositories contain nightly builds as well. In the case your
system is not listed here or you need to modify the sources, [you have to build SUMO from sources](Installing/Linux_Build.md).

# macOS

A guide for Homebrew-based (encouraged) as well as Macports-based
(legacy) installation is given
[here](Installing/MacOS_Build.md).

# via Docker

Building and installing SUMO from source is not an easy task for
beginner users. Docker is a popular tool to solve this issue. Searching
"SUMO" at [Docker Hub](https://hub.docker.com) will give several results
from existing attempts at Dockerising SUMO.

The solution given at
[docker-sumo](https://github.com/bogaotory/docker-sumo) demonstrates how
to Dockerise SUMO version 0.30.0 on top of Ubuntu 16.04. As well as
**sumo** and **traci**, the use of **sumo-gui** is also demonstrated by
[docker-sumo](https://github.com/bogaotory/docker-sumo) so that the
users have access to the graphical interface of a Dockerised SUMO.