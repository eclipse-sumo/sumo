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

# create POILane
netedit.leftClick(match, 140, 215)

# change offset (invalid)
netedit.modifyShapeDefaultValue(2, "dummyOffset")

# try to create POILane
netedit.leftClick(match, 160, 215)

# change offset (empty)
netedit.modifyShapeDefaultValue(2, "")

# try to create POILane
netedit.leftClick(match, 160, 215)

# change offset (valid, negative)
netedit.modifyShapeDefaultValue(2, "-4")

# create POILane
netedit.leftClick(match, 160, 215)

# change offset (valid, positive)
netedit.modifyShapeDefaultValue(2, "6")

# create POILane
netedit.leftClick(match, 180, 215)

# change color using dialog
netedit.changeColorUsingDialog(3, 5)

# create POILane
netedit.leftClick(match, 200, 215)

# change color manually (invalid)
netedit.modifyShapeDefaultValue(4, "Vlue")

# try to create POILane
netedit.leftClick(match, 220, 215)

# change color manually (valid)
netedit.modifyShapeDefaultValue(4, "blue")

# create POILane
netedit.leftClick(match, 220, 215)

# change type
netedit.modifyShapeDefaultValue(5, "poi_test")

# create POILane
netedit.leftClick(match, 240, 215)

# change layer (invalid)
netedit.modifyShapeDefaultValue(6, "dummyLayer")

# try to create POILane
netedit.leftClick(match, 260, 215)

# change layer (valid, negative)
netedit.modifyShapeDefaultValue(6, "-2")

# create POILane
netedit.leftClick(match, 260, 215)

# change layer (valid)
netedit.modifyShapeDefaultValue(6, "3")

# create POILane
netedit.leftClick(match, 280, 215)

# change width (invalid, dummy)
netedit.modifyShapeDefaultValue(7, "dummyWidth")

# try to create POILane
netedit.leftClick(match, 300, 215)

# change width (invalid, negative)
netedit.modifyShapeDefaultValue(7, "-2")

# try to create POILane
netedit.leftClick(match, 300, 215)

# change width (valid)
netedit.modifyShapeDefaultValue(7, "2")

# create POILane
netedit.leftClick(match, 300, 215)

# change height (invalid, dummy)
netedit.modifyShapeDefaultValue(8, "dummyHeight")

# try to create POILane
netedit.leftClick(match, 320, 215)

# change height (invalid, negative)
netedit.modifyShapeDefaultValue(8, "-3")

# try to create POILane
netedit.leftClick(match, 320, 215)

# change height (valid)
netedit.modifyShapeDefaultValue(8, "2")

# create POILane
netedit.leftClick(match, 320, 215)

# change color to white (To show image)
netedit.modifyShapeDefaultValue(4, "white")

# change imgfile (invalid)
netedit.modifyShapeDefaultValue(9, "%%$%$&$%$%$")

# try to create POILane
netedit.leftClick(match, 340, 215)

# change imgfile (non exist)
netedit.modifyShapeDefaultValue(9, "paris.ico")

# try to create POILane
netedit.leftClick(match, 340, 215)

# change imgfile (valid)
netedit.modifyShapeDefaultValue(9, "berlin_icon.ico")

# create POILane
netedit.leftClick(match, 340, 215)

# change save relative path
netedit.modifyShapeDefaultBoolValue(10)

# create POILane
netedit.leftClick(match, 360, 215)

# change angle (invalid)
netedit.modifyShapeDefaultValue(11, "dummyAngle")

# try to create POILane
netedit.leftClick(match, 380, 215)

# change angle (invalid, dummy)
netedit.modifyShapeDefaultValue(11, "dummy")

# try to create POILane
netedit.leftClick(match, 380, 215)

# change angle (invalid, empty)
netedit.modifyShapeDefaultValue(11, "")

# try to create POILane
netedit.leftClick(match, 380, 215)

# change angle (valid, > 360)
netedit.modifyShapeDefaultValue(11, "500")

# create POILane
netedit.leftClick(match, 380, 215)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(11, "-27")

# create POILane
netedit.leftClick(match, 400, 215)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(11, "45")

# create POILane
netedit.leftClick(match, 420, 215)

# change block
netedit.modifyShapeDefaultBoolValue(13)

# create POILane
netedit.leftClick(match, 440, 215)

# Check undo redo
netedit.undo(match, 17)
netedit.redo(match, 17)

# save shapes
netedit.saveShapes()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
