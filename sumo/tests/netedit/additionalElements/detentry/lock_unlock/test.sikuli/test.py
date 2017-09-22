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

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3
netedit.leftClick(match, 250, 50)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 100, 200)

# change to move mode
netedit.moveMode()

# move Entry to right
netedit.moveElement(match, -75, 190, 150, 190)

# go to inspect mode
netedit.inspectMode()

# inspect Entry
netedit.leftClick(match, 275, 190)

# block additional
netedit.modifyBoolAttribute(3)

# change to move mode
netedit.moveMode()

# try to move Entry to right (must be blocked)
netedit.moveElement(match, 140, 190, 300, 190)

# go to inspect mode
netedit.inspectMode()

# inspect Entry
netedit.leftClick(match, 275, 190)

# unblock additional
netedit.modifyBoolAttribute(3)

# change to move mode
netedit.moveMode()

# move Entry to right (must be allowed)
netedit.moveElement(match, 140, 190, 300, 190)

# Check undos and redos
netedit.undo(match, 6)
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
