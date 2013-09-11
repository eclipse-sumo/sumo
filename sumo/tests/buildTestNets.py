#!/usr/bin/env python
"""
@file    buildTestNets.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@version $Id$

Rebuils all sumo networks serving as input for the tests.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, sys, subprocess

mRoot = "."
if len(sys.argv)>1:
    mRoot = sys.argv[1]
binPrefix = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'bin', 'net')
for root, dirs, files in os.walk(mRoot):
    if ".svn" in dirs:
        dirs.remove(".svn")
    for file in files:
        if file.endswith(".netccfg") or file.endswith(".netgcfg"):
            exe = binPrefix + "generate"
            if file.endswith(".netccfg"):
                exe = binPrefix + "convert"
            print "----------------------------------"
            print "Rebuilding config: " + os.path.join(root, file)
            curDir = os.getcwd()
            os.chdir(root)
            subprocess.call([exe, "--save-configuration", file+".tmp", "-c", file], stdout=sys.stdout, stderr=sys.stderr)
            os.remove(file)
            os.rename(file+".tmp", file)
            os.chdir(curDir)
            print "Running: " + file
            subprocess.call([exe, "-v", "-c", os.path.join(root, file)], stdout=sys.stdout, stderr=sys.stderr)
            print "----------------------------------\n"
