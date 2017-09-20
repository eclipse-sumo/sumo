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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, ['--new'])

# zoom
netedit.setZoom("50", "50", "50")

# rebuild network
netedit.rebuildNetwork()

# Change to create mode
netedit.createEdgeMode()

# Create two nodes
netedit.leftClick(match, 100, 200)
netedit.leftClick(match, 500, 200)

# Create another two nodes
netedit.leftClick(match, 100, 400)
netedit.leftClick(match, 500, 400)

# select two-way mode
netedit.changeTwoWayOption()

# create square
netedit.leftClick(match, 100, 400)
netedit.leftClick(match, 100, 200)
netedit.leftClick(match, 500, 200)
netedit.leftClick(match, 500, 400)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# rebuild network
netedit.rebuildNetwork()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
