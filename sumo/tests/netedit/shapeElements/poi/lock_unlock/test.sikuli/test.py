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

# go to additional mode
netedit.changeShape("poi")

# create poi
netedit.leftClick(match, 100, 100)

# change color to white (To see icon)
netedit.modifyShapeDefaultValue(3, "white")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "10")

# Change parameter 6 with a valid value
netedit.modifyAttribute(7, "10")

# change imgfile (valid)
netedit.modifyShapeDefaultValue(8, "berlin_icon.ico")

# create poi
netedit.leftClick(match, 100, 350)

# go to inspect mode
netedit.inspectMode()

# inspect first POI
netedit.leftClick(match, 100, 100)

# block POI
netedit.modifyBoolAttribute(10)

# inspect first POI
netedit.leftClick(match, 100, 350)

# block POI
netedit.modifyBoolAttribute(10)

# go to move mode
netedit.moveMode()

# try to move first POI to left down
netedit.moveElement(match, -80, 55, 200, 60)

# try to move second POI to left up
netedit.moveElement(match, -80, 400, 200, 300)

# go to inspect mode again
netedit.inspectMode()

# inspect first POI
netedit.leftClick(match, 100, 100)

# unblock POI
netedit.modifyBoolAttribute(10)

# inspect first POI
netedit.leftClick(match, 100, 350)

# unblock POI
netedit.modifyBoolAttribute(10)

# go to move mode
netedit.moveMode()

# move first POI to left down
netedit.moveElement(match, -80, 55, 200, 60)

# move second POI to left up
netedit.moveElement(match, -80, 400, 200, 300)

# Check undo redo
netedit.undo(match, 2)
netedit.redo(match, 2)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
