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

# create E3 with default parameters
netedit.leftClick(match, 275, 50)

# select entry detector
netedit.changeAdditional("detEntry")

# try to create Entry without select child
netedit.leftClick(match, 50, 200)

# Create three Entry detectors
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 50, 200)
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 200, 200)
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 350, 200)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(1)

# create entry detector with different friendly pos
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 500, 200)

# Check undo redo
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
