#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

python script used by sikulix for testing netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR/TS, Germany

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
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

# select POI in list of shapes
netedit.changeShape("poi")

# create first POI
netedit.leftClick(match, 100, 50)

# create second POI
netedit.leftClick(match, 150, 50)

# go to inspect mode
netedit.inspectMode()

# inspect first POI
netedit.leftClick(match, 100, 50)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "POI_1")

# Change parameter 0 with a non valid value (empty)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a non valid value (invalid)
netedit.modifyAttribute(0, "ID with spaces")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "newID")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyPosition")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "20.40,7.50")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummyColor")

# Change parameter 2 with a non valid value (invalid format)
netedit.modifyAttribute(2, "255,255,500")

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(2, "blue")

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(2, "125,60,200")

# Change parameter 3 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyValue")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "gneE1_0")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "myOwnType")

# Change parameter 5 with a non valid value
netedit.modifyAttribute(5, "dummyLayer")

# Change parameter 5 with a valid value (negative)
netedit.modifyAttribute(5, "-2")

# Change parameter 5 with a valid value (negative)
netedit.modifyAttribute(5, "2")

# Change parameter 6 with a non valid value (dummy)
netedit.modifyAttribute(6, "dummyWidth")

# Change parameter 6 with a non valid value (negative)
netedit.modifyAttribute(6, "-2")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "5")

# Change parameter 7 with a non valid value (dummy)
netedit.modifyAttribute(7, "dummyHeight")

# Change parameter 7 with a non valid value (negative)
netedit.modifyAttribute(7, "-3")

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "5")

# Change parameter 8 with a non valid value (invalid)
netedit.modifyAttribute(8, "%$$%%%%%")

# Change parameter 8 with a non valid value (no exist)
netedit.modifyAttribute(8, "paris.ico")

# Change parameter 8 with a valid value (valid)
netedit.modifyAttribute(8, "berlin_icon.ico")

# Change parameter 9 with a non valid value (dummy)
netedit.modifyAttribute(9, "dummyAngle")

# Change parameter 9 with a valid value (negative)
netedit.modifyAttribute(9, "-12")

# Change parameter 9 with a valid value (> 360º)
netedit.modifyAttribute(9, "500")

# Change parameter 9 with a valid value (> 360º)
netedit.modifyAttribute(9, "30")

# Change boolean parameter 3
netedit.modifyShapeDefaultBoolValue(10)

# Check undos and redos
netedit.undo(match, 25)
netedit.redo(match, 25)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
