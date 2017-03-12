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
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, False)

# apply zoom
netedit.zoomIn(match.getTarget().offset(325, 200), 10)

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(match, 100, 100)

# create E3 2
netedit.leftClick(match, 200, 100)

# select Exit detector
netedit.changeAdditional("detExit")

# Create Exit detector for E3 1
netedit.selectAdditionalChild(4, 2)
netedit.leftClick(match, 75, 250)

# Create Exit detectors for E3 2
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 250, 250)

# Change to delete
netedit.deleteMode()

# delete created E3 1
netedit.leftClick(match, 100, 100)

# delete Exit detector of E3 2 (E3 will not be removed)
netedit.leftClick(match, 250, 250)

# delete loaded E3 1
netedit.leftClick(match, 400, 100)

# delete Exit detector of loaded E3 2 (E3 will not be removed)
netedit.leftClick(match, 500, 250)

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
