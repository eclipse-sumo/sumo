#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    buildTestNets.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2007-04-03
# @version $Id$

import os
import sys
import subprocess

mRoot = "."
if len(sys.argv) > 1:
    mRoot = sys.argv[1]
binPrefix = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), '..', 'bin', 'net')
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
            subprocess.call([exe, "--save-configuration", file +
                             ".tmp", "-c", file], stdout=sys.stdout, stderr=sys.stderr)
            os.remove(file)
            os.rename(file + ".tmp", file)
            os.chdir(curDir)
            print "Running: " + file
            subprocess.call(
                [exe, "-v", "-c", os.path.join(root, file)], stdout=sys.stdout, stderr=sys.stderr)
            print "----------------------------------\n"
