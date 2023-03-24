#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to shape mode
netedit.shapeMode()

# go to shape mode
netedit.changeElement("poiGeo")

# create poi
netedit.leftClick(referencePosition, 100, 100)

# change color to white (To see icon)
netedit.changeDefaultValue(netedit.attrs.POIGEO.create.color, "white")

# Change parameter width with a valid value (To see icon)
netedit.changeDefaultValue(netedit.attrs.POIGEO.create.width, "10")

# Change parameter height with a valid value (To see icon)
netedit.changeDefaultValue(netedit.attrs.POIGEO.create.height, "10")

# change imgfile (valid)
netedit.changeDefaultValue(netedit.attrs.POIGEO.create.imgFile, "berlin_icon.ico")

# create poi
netedit.leftClick(referencePosition, 100, 350)

# go to move mode
netedit.moveMode()

# move first POI to left down
netedit.moveElement(referencePosition, 108, 94, 375, 151)

# move second POI to left up
netedit.moveElement(referencePosition, 110, 363, 355, 250)

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
