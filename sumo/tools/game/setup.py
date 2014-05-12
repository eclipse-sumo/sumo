"""
@file    setup.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2010-05-23
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from distutils.core import setup
import py2exe, sys, shutil, os, glob, zipfile
import subprocess, tempfile

nightlyDir="O:\\Daten\\Sumo\\Nightly"

if len(sys.argv) == 1:
    sys.argv.append("py2exe")
internal = False
if "internal" in sys.argv:
    internal = True
    sys.arg.remove["internal"]

base = os.path.abspath(os.path.dirname(__file__))
oldDir = os.getcwd()
tmpDir = tempfile.mkdtemp()
os.chdir(tmpDir)
os.mkdir("dist")

setup(console=[os.path.join(base, 'runner.py')])

for f in glob.glob(os.path.join(base, "*.sumocfg")): 
    shutil.copy2(f, "dist")
for f in ['input_additional.add.xml', 'logo.gif', 'dlr.gif']:
    shutil.copy2(os.path.join(base, f), "dist")
for dir in ['cross', 'square', 'kuehne', 'highway', 'sounds', 'ramp', 'bs3d']:
    subprocess.call(['svn', 'export', os.path.join(base, dir), os.path.join("dist", dir)])
for dll in glob.glob(os.path.join(nightlyDir, "*.dll")):
    shutil.copy2(dll, "dist")
if internal:
    pluginDir = glob.glob(os.path.join(nightlyDir, "osgPlugins*"))[0]
    shutil.copytree(pluginDir, os.path.join("dist", os.path.basename(pluginDir)))
    shutil.copy2(os.path.join(nightlyDir, "meso-gui.exe"), "dist")
else:
    shutil.copy2(os.path.join(nightlyDir, "sumo-gui.exe"), "dist")

zipf = zipfile.ZipFile(os.path.join(nightlyDir, "sumogame.zip"), 'w', zipfile.ZIP_DEFLATED)
root_len = len(os.path.abspath("dist"))
for root, dirs, files in os.walk("dist"):
    archive_root = os.path.abspath(root)[root_len:]
    for f in files:
        fullpath = os.path.join(root, f)
        archive_name = os.path.join(archive_root, f)
        zipf.write(fullpath, archive_name)
zipf.close()
os.chdir(oldDir)
shutil.rmtree(tmpDir, True)
