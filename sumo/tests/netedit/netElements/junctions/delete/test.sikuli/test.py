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


# rebuild network
netedit.rebuildNetwork()

# Change to delete mode
netedit.deleteMode()

# remove one way edge
netedit.leftClick(match, 50, 50)

# remove two way edges
netedit.leftClick(match, 260, 50)

# remove two way edges
netedit.leftClick(match, 500, 50)

# remove square
netedit.leftClick(match, 60, 160)
netedit.leftClick(match, 150, 280)

# remove circular road
netedit.leftClick(match, 450, 270)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 6)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
