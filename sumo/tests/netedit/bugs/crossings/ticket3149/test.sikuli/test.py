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

# zoom in central node
netedit.setZoom("50", "50", "100")

# Rebuild network
netedit.rebuildNetwork()

# go to inspect mode
netedit.inspectMode()


# select first left edge and change their junction
netedit.leftClick(match, 180, 200)
netedit.modifyAttribute(1, "B")
netedit.rebuildNetwork()

# select second left edge and change their junction
netedit.leftClick(match, 180, 250)
netedit.modifyAttribute(2, "A")
netedit.rebuildNetwork()

# select first right edge and change their junction
netedit.leftClick(match, 450, 200)
netedit.modifyAttribute(2, "B")
netedit.rebuildNetwork()

# select second right edge and change their junction
netedit.leftClick(match, 450, 250)
netedit.modifyAttribute(1, "A")
netedit.rebuildNetwork()

# Check undo redo
netedit.undo(match, 4)
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 4)
netedit.rebuildNetwork()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
