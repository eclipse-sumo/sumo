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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# zoom in central node
netedit.setZoom("50", "50", "50")

# Rebuild network
netedit.rebuildNetwork()

# set crossing mode
netedit.crossingMode()

# select central node
netedit.leftClick(match, 325, 225)

# select a single edge edges and create a split crossing
netedit.leftClick(match, 150, 200)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 150, 250)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 500, 200)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 500, 250)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 250, 70)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 350, 70)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 250, 400)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 350, 400)
netedit.createCrossing()
netedit.rebuildNetwork()

# Check undo redo
netedit.undo(match, 8)
netedit.redo(match, 8)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
