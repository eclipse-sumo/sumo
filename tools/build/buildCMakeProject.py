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
import argparse

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.dirname(os.path.dirname(os.path.dirname(__file__))))
PATH = os.environ["PATH"].split(os.pathsep)
CMAKE = []
for folder in PATH:
    cmakeExe = os.path.join(folder, "cmake.exe")
    if os.path.exists(cmakeExe):
        CMAKE.append(cmakeExe)
CMAKE += glob.glob("C:/Program*/CMake/bin/cmake.exe")

def generate(generator, log=sys.stdout):
    # First check that CMake was correctly installed
    if len(CMAKE) == 0:
        print("""CMake executable wasn't found.
        Please install the last version of Cmake from https://cmake.org/download/,
        or add the folder of cmake executable to PATH""", file=log)
        return None
    print("Searching libraries...", file=log)
    cmakeOpt = ["-DBUILD_GTEST_FROM_GIT=true"]
    # append custom lib dir to search path
    libFolders = []
    for libDir in (os.path.join(SUMO_HOME, "lib"), os.path.join(SUMO_HOME, "SUMOLibraries"),
                   os.path.join(SUMO_HOME, "..", "SUMOLibraries"), os.path.join(SUMO_HOME, "..", "..", "SUMOLibraries")):
        if os.path.exists(libDir):
            libFolders += [os.path.join(libDir, f, "bin") for f in os.listdir(libDir)]
    for folder in libFolders + PATH:
        if generator.endswith("Win64"):
            if "_64" not in folder and folder.replace("\\bin", "_64\\bin") in libFolders:
                continue
        else:
            if "_64" in folder and folder.replace("_64", "") in libFolders:
                continue
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
        elif "swig" in folder.lower():
            cmakeOpt += ["-DSWIG_EXECUTABLE=%sswig.exe" % folder[:-3]]

    buildDir = os.path.join(SUMO_HOME, "build", "cmake-build-" + generator.replace(" ", "-"))
    # Create directory or clear it if already exists
    if os.path.exists(buildDir):
        print("Cleaning directory of", generator, file=log)
        # cannot use shutil.rmtree here, since it does not clean up the .git dirs
        subprocess.call("rmdir /S /Q " + buildDir, shell=True, stdout=log, stderr=subprocess.STDOUT)
    os.makedirs(buildDir)
    print("Creating solution for", generator, file=log)
    subprocess.call([CMAKE[0], "../..", "-G", generator] + cmakeOpt, cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
    return buildDir

def build(buildDir, config, log=sys.stdout):
    subprocess.call([CMAKE[0], "--build", ".", "--config", config], cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--generator", default="Visual Studio 12 2013 Win64",
                        help="the Visual Studio version or Eclipse project you are targeting, see cmake --help")
    parser.add_argument("--config", help="trigger an immediate build of the given configuration")
    args = parser.parse_args()
    buildDir = generate(args.generator)
    if buildDir is not None and args.config is not None:
        build(buildDir, args.config)
