#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id: test.py 25910 2017-09-07 13:49:36Z namdre $

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

# go to poly mode and select poly
netedit.changeShape("poly")

# create first polygon
netedit.createSquaredPoly(match, 200, 150, 200, True)

# create second polygon
netedit.createSquaredPoly(match, 400, 150, 200, True)

# go to inspect mode
netedit.inspectMode()

# inspect first polygon
netedit.leftClick(match, 100, 50)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "poly_1")

# Change parameter 0 with a non valid value (empty)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a non valid value (invalid)
netedit.modifyAttribute(0, "ID with spaces")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "newID")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyShape")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (single point)
netedit.modifyAttribute(1, "12.00,8.00")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "12.00,8.00 9.00,13.00 12.00,18.00 7.00,15.00 2.00,18.00 5.00,13.00 2.00,8.00 7.00,11.00 12.00,8.00")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummyColor")

# Change parameter 2 with a non valid value (invalid format)
netedit.modifyAttribute(2, "255,255,500")

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(2, "blue")

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(2, "125,60,200")

# Change boolean parameter 3
netedit.modifyShapeDefaultBoolValue(3)

# Change parameter 4 with a non valid value
netedit.modifyAttribute(4, "dummyLayer")

# Change parameter 4 with a valid value (negative)
netedit.modifyAttribute(4, "-2")

# Change parameter 4 with a valid value (negative)
netedit.modifyAttribute(4, "2")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "myOwnType")

# Change parameter 6 with a non valid value (invalid)
netedit.modifyAttribute(6, "%$$%%%%%")

# Change parameter 6 with a non valid value (non exist)
netedit.modifyAttribute(6, "paris.ico")

# Change parameter 8 with a valid value (valid)
netedit.modifyAttribute(6, "berlin_icon.ico")

# Change parameter 7 with a non valid value (dummy)
netedit.modifyAttribute(7, "dummyAngle")

# Change parameter 7 with a valid value (negative)
netedit.modifyAttribute(7, "-5")

# Change parameter 7 with a valid value (> 360)
netedit.modifyAttribute(7, "365")

# Change parameter 7 with a valid value (> 360)
netedit.modifyAttribute(7, "10")

# Change boolean parameter 3
netedit.modifyShapeDefaultBoolValue(8)

# Change boolean parameter 3
netedit.modifyShapeDefaultBoolValue(9)

# Change boolean parameter 3
netedit.modifyShapeDefaultBoolValue(10)

# Check undos and redos
netedit.undo(match, 22)
netedit.redo(match, 22)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)