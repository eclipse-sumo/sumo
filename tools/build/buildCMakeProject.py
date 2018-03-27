#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    buildCMakeProject.py
# @author  Pablo Alvarez Lopez
# @author  Michael Behrisch
# @date    2017
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
import shutil
import glob

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
pathFolders = os.environ["PATH"].split(os.pathsep)
CMAKE = []
for folder in pathFolders:
    cmakeExe = os.path.join(folder, "cmake.exe")
    if os.path.exists(cmakeExe):
        CMAKE.append(cmakeExe)
CMAKE += glob.glob("C:/Program*/CMake/bin/cmake.exe")
# First check that CMake was correctly installed
if len(CMAKE) == 0:
    print("""CMake executable wasn't found.
    Please install the last version of Cmake from https://cmake.org/download/,
    or add the folder of cmake executable to PATH""")
else:
    print("Searching libraries...")

    # append custom lib dir to search path
    for libDir in (os.path.join(SUMO_HOME, "lib"), os.path.join(SUMO_HOME, "SUMOLibraries"),
                   os.path.join(SUMO_HOME, "..", "SUMOLibraries"), os.path.join(SUMO_HOME, "..", "..", "SUMOLibraries")):
        if os.path.exists(libDir):
            pathFolders += [os.path.join(libDir, f, "bin") for f in os.listdir(libDir)]
    for folder in pathFolders:
        if "fox-1.6" in folder.lower():
            os.environ["FOX_LIBRARY"] = folder[:-3] + "lib"
            os.environ["FOX_INCLUDE_DIR"] = folder[:-3] + "include"
        elif "xerces-c-3" in folder.lower():
            os.environ["CMAKE_LIBRARY_PATH"] = folder[:-3] + "lib"
            os.environ["CMAKE_INCLUDE_PATH"] = folder[:-3] + "include"
        elif "gdal" in folder.lower():
            os.environ["GDAL_DIR"] = os.path.dirname(folder)
        elif "python27" in folder.lower() and "scripts" not in folder.lower():
            os.environ["PYTHON_LIB"] = folder + "libs\python27.lib"
            os.environ["PYTHON_INCLUDE"] = folder + "include"
        elif "python36" in folder.lower() and "scripts" not in folder.lower():
            os.environ["PYTHON_LIB"] = folder + "libs\python36.lib"
            os.environ["PYTHON_INCLUDE"] = folder + "include"

    generator = sys.argv[1] if len(sys.argv) > 1 else "Visual Studio 12 2013 Win64"
    buildDir = os.path.join(SUMO_HOME, "build/cmake-build-" + generator.replace(" ", "-"))
    # Create directory or clear it if already exists
    if os.path.exists(buildDir):
        print ("Cleaning directory of", generator)
        shutil.rmtree(buildDir)
    os.makedirs(buildDir)
    print ("Creating solution for", generator)
    subprocess.call([CMAKE[0], "../..", "-G", generator, "-DBUILD_GTEST_FROM_GIT=true"], cwd=buildDir)
