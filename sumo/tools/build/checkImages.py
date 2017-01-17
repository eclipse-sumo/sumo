#!/usr/bin/env python
"""
@file    checkImages.py
@author  Michael Behrisch
@date    2016
@version $Id$

Checks whether all images in src/utils/gui/images are properly
 inserted into the Makefile

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
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
