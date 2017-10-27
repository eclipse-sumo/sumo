#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    checkImages.py
# @author  Michael Behrisch
# @date    2016
# @version $Id$

"""
Checks whether all images in src/utils/gui/images are properly
 inserted into the Makefile
"""
from __future__ import absolute_import
from __future__ import print_function

import os

srcRoot = os.path.join(os.path.dirname(__file__), "../../src/utils/gui/images")
extraDist = []
for line in open(os.path.join(srcRoot, "Makefile.am")):
    elems = line.split()
    if line.startswith("EXTRA_DIST"):
        extraDist = [f for f in elems[2:] if f != '\\']
    if extraDist:
        if not elems:
            break
        extraDist += [f for f in elems if f != '\\']

for _, _, files in os.walk(srcRoot):
    for name in files:
        if name.endswith(".xpm") or name.startswith("GNETexture_"):
            if name not in extraDist:
                print(name, "is missing from the Makefile")
    break
