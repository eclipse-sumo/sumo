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

# first rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# first check that invert works
netedit.selectionInvert()

# invert again and delete selected items (all must be unselected)
netedit.selectionInvert()
netedit.deleteSelectedItems()

# invert again and clear selection
netedit.selectionInvert()
netedit.selectionClear()

# use a rectangle to select central elements
netedit.selectionRectangle(match, 250, 150, 400, 300)

# invert selection to select only extern nodes and delete it
netedit.selectionInvert()
netedit.deleteSelectedItems()

# check undo and redo
netedit.undo(match, 1)
netedit.redo(match, 2)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
