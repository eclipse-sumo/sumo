#!/usr/bin/env python
"""
@file    setup.py
@author  Michael Behrisch
@author  Jakob Erdmann
@date    2010-05-23
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

from distutils.core import setup
import py2exe
import sys
import shutil
import os
import glob
import zipfile
import subprocess
import tempfile

for d in ["Program Files", "Program Files (x86)", "Programme"]:
    sevenZip = r'C:\%s\7-Zip\7z.exe' % d
    if os.path.exists(sevenZip):
        break

inZip = os.path.abspath(sys.argv[1])
sys.argv[1] = "py2exe"

base = os.path.abspath(os.path.dirname(__file__))
oldDir = os.getcwd()
tmpDir = tempfile.mkdtemp()
os.chdir(tmpDir)
os.mkdir("dist")

setup(console=[os.path.join(base, 'runner.py')])

for pattern in ['*.sumocfg', 'input_additional.add.xml', '*.gif']:
    for f in glob.glob(os.path.join(base, pattern)):
        shutil.copy2(f, "dist")
for d in os.listdir(base):
    path = os.path.join(base, d)
    if os.path.isdir(path):
        subprocess.call(['svn', 'export', path, os.path.join("dist", d)])
osgPlugins = None
with zipfile.ZipFile(inZip) as binZip:
    for f in binZip.namelist():
        if "osgPlugins" in f:
            extracted = binZip.extract(f)
            if osgPlugins is None:
                if f.endswith("/"):
                    osgPlugins = extracted
                else:
                    osgPlugins = os.path.dirname(extracted)
        elif f.endswith(".dll") or f.endswith("gui.exe") or f.endswith("sumo.exe"):
            extracted = binZip.extract(f)
            dest = os.path.join("dist", os.path.basename(f))
            if os.path.isfile(extracted) and not os.path.exists(dest):
                os.rename(extracted, dest)
os.chdir("dist")
if osgPlugins:
    os.rename(osgPlugins, os.path.basename(osgPlugins))
    for f in glob.glob(os.path.join(base, '..', '..', 'data', '3D', '*')):
        shutil.copy2(f, ".")
    os.chdir("bs3d")
    subprocess.call([sevenZip, 'x', os.path.join(
        os.path.dirname(inZip), '..', '3D_Modell_Forschungskreuzung_BS.7z')])
    os.chdir("..")
zipf = zipfile.ZipFile(
    inZip.replace("sumo-", "sumo-game-"), 'w', zipfile.ZIP_DEFLATED)

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
