#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    buildMSVS17Project64Bits.py
# @author  Pablo Alvarez Lopez
# @date    2017
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import shutil

# First check that CMake was correctly installed
if (os.environ["PATH"].lower().find("cmake") == -1):
    print("""CMake executable wasn't found.
    Please install the last version of Cmake from https://cmake.org/download/,
    or add the folder of cmake executable to PATH""")
else:
    # print debug
    print ("CMake found")

    # start to find libraries
    print ("Searching libraries...")

    # obtain all path of folder
    pathFolders = os.environ["PATH"].split(';')

    # iterate over folder of path
    for folder in pathFolders:
        if (folder.lower().find("fox-1.6") != -1):
            foxLib = folder
            foxFolder = folder[:-3]
            foxInc = folder[:-3] + "include"
        elif (folder.lower().find("xerces-c-3") != -1):
            xercesBin = folder
            xercesLib = folder[:-3] + "lib"
            xercesInc = folder[:-3] + "include"
        elif (folder.lower().find("proj_gdal") != -1):
            gdalBin = folder
            gdalLib = folder[:-3] + "lib"
            gdalInc = folder[:-3] + "include"
        elif (folder.lower().find("python27") != -1 and folder.lower().find("scripts") == -1):
            pythonLib = folder + "libs\python27.lib"
            pythonInc = folder + "include"
        elif (folder.lower().find("python36") != -1 and folder.lower().find("scripts") == -1):
            pythonLib = folder + "libs\python36.lib"
            pythonInc = folder + "include"

    # Check if library was found
    abort = False

    if (foxLib == ""):
        print ("Fox library wasn't found. Check that Fox lib directory was added to PATH")
        abort = True
    else:
        print ("Fox library found")

    if (xercesBin == ""):
        print ("Xerces library wasn't found. Check that Xerces bin directory was added to PATH")
        abort = True
    else:
        print ("Xerces library found")

    if (gdalBin == ""):
        print ("Gdal library wasn't found. Check that Gdal bin directory was added to PATH")
        abort = True
    else:
        print ("Gdal library found")
        
    if (pythonLib == ""):
        print ("Python library wasn't found. Check that python directory was added to PATH")
        abort = True
    else:
        print ("Python library found")

    # If all libraries were found, continue. In other case, abort
    if (abort):
        print ("Could not build Projects, needed libraries weren't found")
    else:
        # print debug
        print ("Setting temporal environment variables.")
        # set temporal environment variables for FOX
        os.environ["FOX_DIR"] = foxFolder
        os.environ["FOX_LIBRARY"] = foxLib
        os.environ["FOX_INCLUDE_DIR"] = foxInc
        # set temporal environment variables for Xerces
        os.environ["XERCES_BIN"] = xercesBin
        os.environ["XERCES_INCLUDE"] = xercesInc
        os.environ["XERCES_LIB"] = xercesLib
        # set temporal environment variables for GDal
        os.environ["GDAL_BIN"] = gdalBin
        os.environ["GDAL_INCLUDE"] = gdalInc
        os.environ["GDAL_LIB"] = gdalLib
        # set temporal environment variables for Python
        os.environ["PYTHON_INCLUDE"] = pythonInc
        os.environ["PYTHON_LIB"] = pythonLib
        # Create directory for VS17, or clear it if already exists
        if not os.path.exists(os.environ["SUMO_HOME"] + "/build/autobuild/msvc17"):
            print ("Creating directory for Visual Studio 2017")
            os.makedirs(os.environ["SUMO_HOME"] + "/build/autobuild/msvc17")
        else:
            print ("cleaning directory of Visual Studio 2017")
            shutil.rmtree(os.environ["SUMO_HOME"] + "/build/autobuild/msvc17")
            os.makedirs(os.environ["SUMO_HOME"] + "/build/autobuild/msvc17")
        # Create solution for visual studio 2017
        print ("Creating solution for Visual Studio 2017")
        VS17Generation = subprocess.Popen(
            "cmake ../../../ -G \"Visual Studio 15 2017\"", cwd=os.environ["SUMO_HOME"] + "/build/autobuild/msvc17")
        # Wait to the end of generation
        VS17Generation.wait()
        
    # Press enter key to finish
    key = input('Press ENTER key to finish')
    quit()