#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id: test.py 25267 2017-07-19 10:41:16Z behrisch $

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

# apply zoom (to see all POIS)
netedit.setZoom("0", "0", "80")

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect POIs
netedit.leftClick(match, 400, 400)

# Set invalid color
netedit.modifyAttribute(0, "Jren")

# Set valid color
netedit.modifyAttribute(0, "green")

# Set type
netedit.modifyAttribute(1, "common type")

# Set invalid layer
netedit.modifyAttribute(2, "dummyLayer")

# Set layer
netedit.modifyAttribute(2, "1")

# Set invalid width
netedit.modifyAttribute(3, "-3")

# Set width
netedit.modifyAttribute(3, "10")

# Set invalid height
netedit.modifyAttribute(4, "-5")

# Set height
netedit.modifyAttribute(4, "10")

# Set invalid imgfile
netedit.modifyAttribute(5, "paris.ico")

# Set imgfile
netedit.modifyAttribute(5, "berlin_icon.ico")

# Set invalid angle
netedit.modifyAttribute(6, "dummyAngle")

# Set valid angle
netedit.modifyAttribute(6, "180")

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
