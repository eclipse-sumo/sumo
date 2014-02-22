#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    traciToHex.py
@author  Michael Behrisch
@date    2010-09-08
@version $Id$

Converts all testclient.prog inputs to hexadecimal values.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
import os, sys

mRoot = "."
if len(sys.argv)>1:
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
