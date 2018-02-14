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

# select POILane in list of shapes
netedit.changeShape("poiLane")

# create POILane
netedit.leftClick(match, 140, 215)

# create POILane
netedit.leftClick(match, 200, 215)

# go to inspect mode
netedit.inspectMode()

# inspect first POILane
netedit.leftClick(match, 140, 215)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "POI_1")

# Change parameter 0 with a non valid value (empty)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a non valid value (invalid)
netedit.modifyAttribute(0, "ID with spaces")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "newID")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyLane")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "gneE0_0")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummyPosition")

# Change parameter 2 with a non valid value (< lane length)
netedit.modifyAttribute(2, "-100")

# Change parameter 2 with a non valid value (> lane length)
netedit.modifyAttribute(2, "200")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "30.5")

# Change parameter 5 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyLatPosition")

# Change parameter 5 with a valid value (negative)
netedit.modifyAttribute(3, "-20")

# Change parameter 5 with a valid value
netedit.modifyAttribute(3, "15")

# Change parameter 4 with a non valid value (dummy)
netedit.modifyAttribute(4, "dummyColor")

# Change parameter 4 with a non valid value (invalid format)
netedit.modifyAttribute(4, "255,255,500")

# Change parameter 4 with a valid value (valid format)
netedit.modifyAttribute(4, "blue")

# Change parameter 4 with a valid value (valid format)
netedit.modifyAttribute(4, "125,60,200")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "myOwnType")

# Change parameter 6 with a non valid value
netedit.modifyAttribute(6, "dummyLayer")

# Change parameter 6 with a valid value (negative)
netedit.modifyAttribute(6, "-2")

# Change parameter 6 with a valid value (negative)
netedit.modifyAttribute(6, "2")

# Change parameter 7 with a non valid value (dummy)
netedit.modifyAttribute(7, "dummyWidth")

# Change parameter 7 with a non valid value (negative)
netedit.modifyAttribute(7, "-2")

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "5")

# Change parameter 8 with a non valid value (dummy)
netedit.modifyAttribute(8, "dummyHeight")

# Change parameter 8 with a non valid value (negative)
netedit.modifyAttribute(8, "-3")

# Change parameter 8 with a valid value
netedit.modifyAttribute(8, "5")

# Change parameter 9 with a non valid value (invalid)
netedit.modifyAttribute(9, "%$$%%%%%")

# Change parameter 9 with a non valid value (no exist)
netedit.modifyAttribute(9, "paris.ico")

# Change parameter 9 with a valid value (valid)
netedit.modifyAttribute(9, "berlin_icon.ico")

# Change boolean parameter 10
netedit.modifyShapeDefaultBoolValue(10)

# Change parameter 11 with a non valid value (dummy)
netedit.modifyAttribute(11, "dummyAngle")

# Change parameter 11 with a valid value (negative)
netedit.modifyAttribute(11, "-12")

# Change parameter 11 with a valid value (> 360)
netedit.modifyAttribute(11, "500")

# Change parameter 11 with a valid value
netedit.modifyAttribute(11, "30")

# Change boolean parameter block
netedit.modifyShapeDefaultBoolValue(13)

# Check undos and redos
netedit.undo(match, 28)
netedit.redo(match, 28)

# save shapes
netedit.saveShapes()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)