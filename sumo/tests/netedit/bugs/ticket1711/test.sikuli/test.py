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

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.setZoom("50", "50", "50")

# set delete mode
netedit.deleteMode()

# remove two left edges
netedit.leftClick(match, 150, 200)
netedit.leftClick(match, 150, 250)

# Rebuild network
netedit.rebuildNetwork()

# remove two right edges
netedit.leftClick(match, 450, 200)
netedit.leftClick(match, 450, 250)

# Rebuild network
netedit.rebuildNetwork()

# remove two up edges
netedit.leftClick(match, 300, 100)
netedit.leftClick(match, 350, 100)

# Rebuild network
netedit.rebuildNetwork()

# remove two down edges
netedit.leftClick(match, 300, 350)
netedit.leftClick(match, 350, 350)

# Rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 8)

# Rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 8)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
