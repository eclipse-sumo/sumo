---
title: Installing/MacOS Build
permalink: /Installing/MacOS_Build/
---

# Using Homebrew

If you come from a previous macports installation you need to uninstall
sumo and fox toolkit first:

```
sudo port uninstall sumo
sudo port uninstall fox
```

If you did not already install [homebrew](http://brew.sh), do so by
invoking

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

make sure your homebrew db is up-to-date

```
brew update
```

Install SUMO stable via the SUMO Brew Formula:

```
brew tap dlr-ts/sumo
brew install sumo
```

Set the **SUMO_HOME** environment variable. To do so, open .bash_profile in TextEdit: 

```
touch ~/.bash_profile; open ~/.bash_profile
```
Just insert the following new line at the end: `export SUMO_HOME=/your/path/to/sumo`, where `/your/path/to/sumo` is the path stated in the caveats section of the `brew install sumo` command.

Restart the Terminal and test the newly added variable:
```
echo $SUMO_HOME
```

After the installation you need to log out/in in order to let X11 start
automatically, when calling a gui-based application like "sumo-gui".
(Alternatively, you may start X11 manually by pressing *cmd-space* and
entering "XQuartz").

# Using Macports (legacy)

!!! note
    This uses a packaged version of sumo which is convenient but may lag behind the latest official release of SUMO.

You should start by [installing
Macports](https://www.macports.org/install.php). Afterwards start a
terminal session and run

```
sudo port install sumo
```

While this will install a SUMO version you maybe do not want to use, it
will pull in all dependencies you need.

If you want to build from a repository checkout you should additionally
do

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

Thanks to all MacOS builders for sharing their insights.
