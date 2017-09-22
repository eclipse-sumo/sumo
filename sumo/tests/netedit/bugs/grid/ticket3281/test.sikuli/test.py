#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

python script used by sikulix for testing netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, ['--new'])

# zoom in central node
netedit.setZoom("100", "0", "200")

# Change to create edge mode
netedit.createEdgeMode()

# Create one way edge
netedit.leftClick(match, -30, 230)
netedit.leftClick(match, 430, 230)

# change to move mode
netedit.moveMode()

# Try to move to origin position
netedit.moveElement(match, 215, 232, -40, 232)

# Now move to top
netedit.moveElement(match, 215, 232, 215, 432)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(match, 3)
netedit.redo(match, 3)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
