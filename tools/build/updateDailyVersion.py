#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    updateDailyVersion.py
# @author  Pablo Alvarez Lopez
# @date    2022-04-26

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import zipfile
import shutil

# first check platform (currently only avaliable in Windows)
if sys.platform != "win32":
    print("This script only works in Windows")
    sys.exit()

# download sumo
print("Downloading daily SUMO...")
os.system('curl https://sumo.dlr.de/daily/sumo-win64extra-git.zip --output sumo.zip')

# now check if sumo.zip exist
if os.path.exists("sumo.zip"):
    print("Download successful. Unzipping ...")
else:
    print("Error downloading SUMO")
    sys.exit()

# unzip SUMO
with zipfile.ZipFile("sumo.zip", 'r') as zip_ref:
    zip_ref.extractall(".")

# check if sumo-git exists
if os.path.exists("sumo-git"):
    print("Unzip successful. Updating SUMO folder ...")
else:
    print("Error unzipping SUMO")
    sys.exit()

# Copy all files
for src_dir, dirs, files in os.walk('./sumo-git'):
    dst_dir = src_dir.replace('./sumo-git', '../../', 1)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
    for file_ in files:
        src_file = os.path.join(src_dir, file_)
        dst_file = os.path.join(dst_dir, file_)
        if os.path.exists(dst_file):
            # in case of the src and dst are the same file
            if os.path.samefile(src_file, dst_file):
                continue
            os.remove(dst_file)
        shutil.move(src_file, dst_dir)

# write info
print("All files copied. Cleaning ...")

# remove temporary files
os.remove("sumo.zip")
shutil.rmtree("sumo-git")

# finished
print("Done.")
