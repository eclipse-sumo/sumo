#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

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
netedit.changeShape("poiLane")

# create poi
netedit.leftClick(referencePosition, 100, 100)

# change color to white (To see icon)
netedit.modifyShapeDefaultValue(3, "white")

# Change parameter 6 with a valid value (To see icon)
netedit.modifyAttribute(6, "10")

# Change parameter 6 with a valid value (To see icon)
netedit.modifyAttribute(7, "10")

# change imgfile (valid)
netedit.modifyShapeDefaultValue(8, "berlin_icon.ico")

# create poi
netedit.leftClick(referencePosition, 100, 350)

# go to inspect mode
netedit.inspectMode()

# inspect first POILane
netedit.leftClick(referencePosition, 100, 100)

# block POILane
netedit.modifyBoolAttribute(13)

# inspect second POILane
netedit.leftClick(referencePosition, 100, 350)

# block POILane
netedit.modifyBoolAttribute(13)

# go to move mode
netedit.moveMode()

# try to move first POILane to left down
netedit.moveElement(referencePosition, -80, 55, 200, 60)

# try to move second POILane to left up
netedit.moveElement(referencePosition, -80, 400, 200, 300)

# go to inspect mode again
netedit.inspectMode()

# inspect first POILane
netedit.leftClick(referencePosition, 100, 100)

# unblock POILane
netedit.modifyBoolAttribute(13)

# inspect first POILane
netedit.leftClick(referencePosition, 100, 350)

# unblock POILane
netedit.modifyBoolAttribute(13)

# go to move mode
netedit.moveMode()

# move first POILane to left down
netedit.moveElement(referencePosition, -80, 55, 200, 60)

# move second POILane to left up
netedit.moveElement(referencePosition, -80, 400, 200, 300)

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save shapes
netedit.saveShapes()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
