#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    buildTestNets.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2007-04-03

from __future__ import print_function
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
            print("----------------------------------")
            print("Rebuilding config: " + os.path.join(root, file))
            sys.stdout.flush()
            curDir = os.getcwd()
            os.chdir(root)
            subprocess.call([exe, "--save-configuration", file +
                             ".tmp", "-c", file], stdout=sys.stdout, stderr=sys.stderr)
            sys.stdout.flush()
            os.remove(file)
            os.rename(file + ".tmp", file)
            os.chdir(curDir)
            print("Running: " + file)
            sys.stdout.flush()
            subprocess.call(
                [exe, "-v", "-c", os.path.join(root, file)], stdout=sys.stdout, stderr=sys.stderr)
            sys.stdout.flush()
            print("----------------------------------\n")
