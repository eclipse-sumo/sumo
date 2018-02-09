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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to shape mode
netedit.shapeMode()

# go to shape mode
netedit.changeShape("poiLane")

# create POILaneLane
netedit.leftClick(match, 140, 215)

# change offset (invalid)
netedit.modifyShapeDefaultValue(1, "dummyOffset")

# try to create POILaneLane
netedit.leftClick(match, 160, 215)

# change offset (empty)
netedit.modifyShapeDefaultValue(1, "")

# try to create POILaneLane
netedit.leftClick(match, 160, 215)

# change offset (valid, negative)
netedit.modifyShapeDefaultValue(1, "-4")

# create POILaneLane
netedit.leftClick(match, 160, 215)

# change offset (valid, positive)
netedit.modifyShapeDefaultValue(1, "6")

# create POILaneLane
netedit.leftClick(match, 180, 215)

# change color using dialog
netedit.changeColorUsingDialog(2, 5)

# create POILaneLane
netedit.leftClick(match, 200, 215)

# change color manually (invalid)
netedit.modifyShapeDefaultValue(3, "Vlue")

# try to create POILaneLane
netedit.leftClick(match, 220, 215)

# change color manually (valid)
netedit.modifyShapeDefaultValue(3, "blue")

# create POILaneLane
netedit.leftClick(match, 220, 215)

# change type
netedit.modifyShapeDefaultValue(4, "poi_test")

# create POILaneLane
netedit.leftClick(match, 240, 215)

# change layer (invalid)
netedit.modifyShapeDefaultValue(5, "dummyLayer")

# try to create POILaneLane
netedit.leftClick(match, 260, 215)

# change layer (valid)
netedit.modifyShapeDefaultValue(5, "2")

# create POILaneLane
netedit.leftClick(match, 260, 215)

# change width (invalid, negative)
netedit.modifyShapeDefaultValue(6, "-2")

# try to create POILaneLane
netedit.leftClick(match, 280, 215)

# change width (valid)
netedit.modifyShapeDefaultValue(6, "2")

# create POILaneLane
netedit.leftClick(match, 280, 215)

# change height (invalid, negative)
netedit.modifyShapeDefaultValue(7, "-3")

# try to create POILaneLane
netedit.leftClick(match, 300, 215)

# change height (valid)
netedit.modifyShapeDefaultValue(7, "2")

# create POILaneLane
netedit.leftClick(match, 300, 215)

# change color to white (To show image)
netedit.modifyShapeDefaultValue(3, "white")

# change imgfile (invalid)
netedit.modifyShapeDefaultValue(8, "%%$%$&$%$%$")

# try to create POILaneLane
netedit.leftClick(match, 320, 215)

# change imgfile (non exist)
netedit.modifyShapeDefaultValue(8, "paris.ico")

# try to create POILaneLane
netedit.leftClick(match, 320, 215)

# change imgfile (valid)
netedit.modifyShapeDefaultValue(8, "berlin_icon.ico")

# create POILaneLane
netedit.leftClick(match, 320, 215)

# change save relative path
netedit.modifyShapeDefaultBoolValue(9)

# create POILaneLane
netedit.leftClick(match, 340, 215)

# change angle (invalid)
netedit.modifyShapeDefaultValue(10, "dummyAngle")

# try to create POILaneLane
netedit.leftClick(match, 360, 215)

# change angle (valid, but > 360)
netedit.modifyShapeDefaultValue(10, "500")

# create POILaneLane
netedit.leftClick(match, 360, 215)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(10, "-27")

# create POILaneLane
netedit.leftClick(match, 380, 215)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(10, "45")

# create POILaneLane
netedit.leftClick(match, 400, 215)

# change block
netedit.modifyShapeDefaultBoolValue(12)

# create POILaneLane
netedit.leftClick(match, 420, 215)

# Check undo redo
netedit.undo(match, 12)
netedit.redo(match, 12)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
