---
title: Linux
---

Python 2.7 comes with most Linux operating systems. Python 3.x is not
yet supported. All required additional packages are available in
repositories:

```
python-numpy
python-wxgtk2.8
python-opengl
python-imaging
python-matplotlib
python-mpltoolkits.basemap
```

However, often Python 3.x is installed along the older version and may
be the default Python interpreter. So make sure you run the sumopy
scripts with Python 2.7

Another issue may that a newer version of `python-wxgtk2.8` has been
included in the repositories of more recent Linux distributions, as for
example Ubuntu-16.04. In this case, do the following safe operation to
install `python-wxgtk2.8`:

```sh
echo "deb http://archive.ubuntu.com/ubuntu wily main universe"| sudo tee /etc/apt/sources.list.d/wily-copies.list

sudo apt install python-wxgtk2.8

sudo rm /etc/apt/sources.list.d/wily-copies.list

sudo apt update
```

SUMOPy now supports wxWidgets 3.0. Here are the steps I needed to do with openSUSE 15.3:
```
sudo zypper addrepo https://download.opensuse.org/repositories/Application:/Geo/openSUSE_Leap_15.3/ Application:Geo
sudo zypper in python-wxWidgets-3_0 python2-opengl python-basemap python2-matplotlib python2-numpy python2-Pillow
sudo pip2 install pyproj
```
Unfortunately pyproj is only available via pypi while python-wxWidgets-3_0 and python-basemap are not available via pypi so you need to use both sources.

You can install pyproj also in a virtualenv if you prefer, then you need to replace the last line with:
```
python2 -m virtualenv --system-site-packages sumopyenv
. sumopyenv/bin/activate
pip install pyproj
```
