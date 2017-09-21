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
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
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

# apply zoom (to see all POIS)
netedit.setZoom("0", "0", "100")

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect POIs
netedit.leftClick(match, 350, 100)

# Set invalid color
netedit.modifyAttribute(0, "Jren")

# Set valid color
netedit.modifyAttribute(0, "green")

# change fill
netedit.modifyBoolAttribute(1)

# Set invalid layer
netedit.modifyAttribute(2, "dummyLayer")

# Set layer
netedit.modifyAttribute(2, "1")

# Set type
netedit.modifyAttribute(3, "common type")

# Set invalid imgfile
netedit.modifyAttribute(4, "paris.ico")

# Set empty imgfile
netedit.modifyAttribute(4, "")

# Set imgfile
netedit.modifyAttribute(4, "berlin_icon.ico")

# Set invalid angle
netedit.modifyAttribute(5, "dummyAngle")

# Set valid angle
netedit.modifyAttribute(5, "180")

# change block move
netedit.modifyBoolAttribute(6)

# change block shape
netedit.modifyBoolAttribute(7)

# change close
netedit.modifyBoolAttribute(8)

# go to select mode
netedit.selectMode()

# clear selection
netedit.selectionClear()

# Check undos and redos
netedit.undo(match, 13)
netedit.redo(match, 13)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
