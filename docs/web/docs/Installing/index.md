---
title: Installing
---

# Windows

There are four different binary packages for Windows depending on the
platform (32 vs. 64 bit) you have and what you want to do with SUMO. If
you want to install it locally and have administrator rights on your
machine you should download and execute one of the installers
(preferably 64 bit). If you need a "portable" version or do not have
admin rights, use the correct zip, extract it into a desired folder
using [7Zip](https://www.7-zip.org/),
[Winzip](https://www.winzip.com/win/de/prod_down.html), or a similar tool. Every
package contains the binaries, all dlls needed, the examples, tools, and
documentation in HTML format.

- Download 64-bit installer: [sumo-win64-{{Version}}.msi](https://sumo.dlr.de/releases/{{Version}}/sumo-win64-{{Version}}.msi)
- Download 64-bit zip: [sumo-win64-{{Version}}.zip](https://sumo.dlr.de/releases/{{Version}}/sumo-win64-{{Version}}.zip)
- Download 64-bit installer with all extras (contains GPL code): [sumo-win64extra-{{Version}}.msi](https://sumo.dlr.de/releases/{{Version}}/sumo-win64extra-{{Version}}.msi)
- Download 64-bit zip with all extras (contains GPL code): [sumo-win64extra-{{Version}}.zip](https://sumo.dlr.de/releases/{{Version}}/sumo-win64extra-{{Version}}.zip)

Within the installation folder, you will find a folder named "**bin**".
Here, you can find the executables (programs). You may double click on
[sumo-gui](../sumo-gui.md) and take a look at the examples located
in **docs/examples**. All other applications
([duarouter](../duarouter.md), [dfrouter](../dfrouter.md),
etc.) have to be run from the command line. To facilitate this there is
also a start-commandline.bat which sets up the whole environment for
you. If you feel unsure about the command line, please read
[Basics/Basic_Computer_Skills\#Running_Programs_from_the_Command_Line](../Basics/Basic_Computer_Skills.md#running_programs_from_the_command_line).

If you want a bleeding edge nightly build or need tests or source files,
you can download them from the [Download](../Downloads.md) page.

For building SUMO from source see [building SUMO under Windows](Windows_Build.md).

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
system is not listed here or you need to modify the sources, [you have to build SUMO from sources](Linux_Build.md).

SUMO's precompiled binary is also available as a Flatpak and is hosted on [Flathub](https://flathub.org/apps/details/org.eclipse.sumo). Any system that supports Flatpak should be able to run SUMO this way. Install Flatpak and add the Flathub repository as [directed here](https://flatpak.org/setup/).

To install SUMO:
```
flatpak install flathub org.eclipse.sumo
```
There should be a SUMO launcher in your menu.

To uninstall SUMO:
```
flatpak remove org.eclipse.sumo
```
Send bug reports regarding SUMO packaged as Flatpak [here](https://github.com/flathub/org.eclipse.sumo/issues).

# macOS

SUMO can be easily installed on macOS by using [Homebrew](http://brew.sh). If you did not already install homebrew, you can do so by invoking te following command in a macOS Terminal:
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```
Please make sure your homebrew installation is up-to-date:
```
brew update
```
If you want to use sumo-gui and/or netedit, you need to install XQuartz as a requirement:
```
brew install --cask xquartz
```
It may be necessary to logout and login again or even reboot to activate the XQuartz integration.
You can then install the latest stable release of SUMO (with minimal requirements: ```fox```, ```proj```, ```xerces-c```) with the following commands:
```
brew tap dlr-ts/sumo
brew install sumo
```
If you need to compile SUMO with support for other libraries, such as GDAL, you need to provide further command line options [as described here](https://github.com/DLR-TS/homebrew-sumo#usage).
To finalize your setup, please make sure to set the **SUMO_HOME** environment variable and have it point to the directory of your SUMO installation. Depending on your shell, you may set this variable either in `.bashrc` or `.zshrc`. To set this variable in `.bashrc` you can use the following commands. 
```
touch ~/.bashrc; open ~/.bashrc
```
Just insert the following new line at the end of the file: 
```
export SUMO_HOME=/your/path/to/sumo
```
where `/your/path/to/sumo` is the path stated in the caveats section of the `brew install sumo` command. Restart the Terminal (or run `source ~/.bashrc`) and test the newly added variable:
```
echo $SUMO_HOME
```
After the installation you need to log out/in in order to let X11 start automatically, when calling a gui-based application like ```sumo-gui```. (Alternatively, you may start X11 manually by pressing *cmd-space* and entering ```XQuartz```).

Additionally, SUMO provides native **macOS application bundles** for its graphical applications, so they can be added to the macOS dock. There is a separate brew cask that will copy these bundles to the `Applications` folder:
```
brew install --cask sumo-gui
```

In case this process fails, it can also be manually achieved by copying these application bundles from `$SUMO_HOME/build/osx/sumo-gui`, `$SUMO_HOME/build/osx/netedit` and `$SUMO_HOME/build/osx/osm-web-wizard` to the `/Applications` folder. Another alternative is to download the application launchers from [here](../Downloads.md#application_launchers).

These application bundles determine the location of your SUMO installation by evaluating your `$SUMO_HOME` variable setting and start the programs accordingly. Multiple SUMO installations may be used by changing the `$SUMO_HOME` variable.

!!! note
    When using these launchers for the first time, macOS may need you to authorize them to run.

## macOS Troubleshooting

### Segmentation faults on macOS Catalina
If you encounter segmentation faults on macOS Catalina, please follow the following steps (see [Issue 6242](https://github.com/eclipse/sumo/issues/6242#issuecomment-553458710)).

1. Uninstall Catalina bottle of fox:
```brew uninstall --ignore-dependencies fox```

2. Edit brew Formula of fox:
```brew edit fox```

3. Comment out or delete the following line:
```sha256 "c6697be294c9a0458580564d59f8db32791beb5e67a05a6246e0b969ffc068bc" => :catalina```

4. Install Mojave bottle of fox:
```brew install fox```

### Blank screen after update to XQuartz 2.8.0_beta3
If you encounter a blank screen after opening sumo-gui (cf. https://github.com/eclipse/sumo/issues/8208), try resetting your DISPLAY environment variable:

```export DISPLAY=:0.0```

# Additional Tools
SUMO contains lots of [Python tools](../Tools/index.md) which are part of the sources and the Windows distribution
and may be packaged on Linux in the sumo or a separate sumo-tools package. To take full advantage of these tools
you need several additional python modules. The easiest way to install them is to use `pip install -r $SUMO_HOME/tools/requirements.txt`.

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
