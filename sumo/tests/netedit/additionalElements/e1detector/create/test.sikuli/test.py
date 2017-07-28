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

# go to additional mode
netedit.additionalMode()

# select E1
netedit.changeAdditional("e1Detector")

# create E1 with default parameters
netedit.leftClick(match, 200, 250)

# set a invalid  frequency
netedit.modifyAdditionalDefaultValue(2, "-30")

# try to create E1 with different frequency
netedit.leftClick(match, 250, 250)

# set a valid frequency
netedit.modifyAdditionalDefaultValue(2, "150")

# create E1 with different frequency
netedit.leftClick(match, 250, 250)

# Change split by type
netedit.modifyAdditionalDefaultBoolValue(3)

# create E1 with different split by type
netedit.leftClick(match, 300, 250)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(4)

# create E1 with different split by type
netedit.leftClick(match, 350, 250)

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
