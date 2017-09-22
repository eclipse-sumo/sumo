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

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeAdditional("e2Detector")

# create E2
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move E2 to left
netedit.moveElement(match, 120, 250, 50, 250)

# move back
netedit.moveElement(match, 50, 250, 120, 250)

# move E2 to right
netedit.moveElement(match, 120, 250, 250, 250)

# move back
netedit.moveElement(match, 250, 250, 120, 250)

# move E2 to left overpassing lane
netedit.moveElement(match, 120, 250, -150, 250)

# move back
netedit.moveElement(match, -80, 250, 120, 250)

# move E2 to right overpassing lane
netedit.moveElement(match, 120, 250, 580, 250)

# move back to another different position of initial
netedit.moveElement(match, 500, 250, 300, 250)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
