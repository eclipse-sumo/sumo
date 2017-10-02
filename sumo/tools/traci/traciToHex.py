#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    traciToHex.py
# @author  Michael Behrisch
# @date    2010-09-08
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

mRoot = "."
if len(sys.argv) > 1:
    mRoot = sys.argv[1]
for root, dirs, files in os.walk(mRoot):
    if ".svn" in dirs:
        dirs.remove(".svn")
    for file in files:
        if file == "testclient.prog":
            full = os.path.join(root, file)
            out = open(full + ".hex", 'w')
            change = False
            for line in open(full):
                l = line.split()
                if l and l[0] in ["setvalue", "getvalue", "getvariable", "getvariable_plus"]:
                    if not l[1][:2] == "0x":
                        l[1] = "0x%x" % int(l[1])
                        change = True
                    if not l[2][:2] == "0x":
                        l[2] = "0x%x" % int(l[2])
                        change = True
                print(" ".join(l), file=out)
            out.close()
            if change:
                if os.name != "posix":
                    os.remove(full)
                os.rename(out.name, full)
            else:
                os.remove(out.name)
