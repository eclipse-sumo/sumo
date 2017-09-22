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

# Change to create edge mode
netedit.createEdgeMode()

# select two-way mode
netedit.changeTwoWayOption()

# select chain mode
netedit.changeChainOption()

# create a circular road
netedit.leftClick(match, 300, 150)
netedit.leftClick(match, 400, 150)
netedit.leftClick(match, 400, 250)
netedit.leftClick(match, 400, 350)
netedit.leftClick(match, 300, 350)
netedit.leftClick(match, 200, 350)
netedit.leftClick(match, 200, 250)
netedit.cancelEdge()

# go to select mode
netedit.selectMode()

# select all elements using invert operation
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect set of junctions
netedit.leftClick(match, 400, 150)

# Set all Junctions as traffic lighs
netedit.modifyAttribute(0, "traffic_light")

# inspect set of edges
netedit.leftClick(match, 450, 150)

# change all speed of edges
netedit.modifyAttribute(0, "20")

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(match, 8)
netedit.redo(match, 8)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
