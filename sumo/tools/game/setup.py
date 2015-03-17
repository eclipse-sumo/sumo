"""
@file    setup.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2010-05-23
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from distutils.core import setup
import py2exe, sys, shutil, os, glob, zipfile
import subprocess, tempfile

nightlyDir=r"O:\Daten\Sumo\Nightly"
sevenZip = r'C:\Program Files\7-Zip\7z.exe'
if not os.path.exists(sevenZip):
    sevenZip = r'C:\Program Files (x86)\7-Zip\7z.exe'

internal = False
if "internal" in sys.argv:
    internal = True
    sys.argv.remove("internal")
if len(sys.argv) == 1:
    sys.argv.append("py2exe")

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
os.chdir("dist")
if internal:
    for dll in glob.glob(os.path.join(nightlyDir, 'bin64', '*.dll')):
        shutil.copy2(dll, ".")
    pluginDir = glob.glob(os.path.join(nightlyDir, 'bin64', 'osgPlugins*'))[0]
    shutil.copytree(pluginDir, os.path.basename(pluginDir))
    shutil.copy2(os.path.join(nightlyDir, 'bin64', 'meso-gui64.exe'), ".")
    for f in glob.glob(os.path.join(base, '..', '..', 'data', '3D', '*')):
        shutil.copy2(f, ".")
    os.chdir("bs3d")
    subprocess.call([sevenZip, 'x', os.path.join(nightlyDir, '..', '3D_Modell_Forschungskreuzung_BS.7z')])
    os.chdir("..")
    zipf = zipfile.ZipFile(os.path.join(nightlyDir, "sumo-game-internal.zip"), 'w', zipfile.ZIP_DEFLATED)
else:
    for dll in glob.glob(os.path.join(nightlyDir, "*.dll")):
        shutil.copy2(dll, ".")
    shutil.copy2(os.path.join(nightlyDir, "sumo-gui.exe"), ".")
    zipf = zipfile.ZipFile(os.path.join(nightlyDir, "sumo-game.zip"), 'w', zipfile.ZIP_DEFLATED)

root_len = len(os.path.abspath("."))
for root, dirs, files in os.walk("."):
    archive_root = os.path.abspath(root)[root_len:]
    for f in files:
        fullpath = os.path.join(root, f)
        archive_name = os.path.join(archive_root, f)
        zipf.write(fullpath, archive_name)
zipf.close()
os.chdir(oldDir)
shutil.rmtree(tmpDir, True)
