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

# create E2 with default parameters
netedit.leftClick(match, 125, 250)

# set invalid  lenght
netedit.modifyAdditionalDefaultValue(2, "-12")

# try to create E2 with different lenght
netedit.leftClick(match, 250, 250)

# set valid lenght
netedit.modifyAdditionalDefaultValue(2, "5")

# create E2 with different lenght
netedit.leftClick(match, 250, 250)

# set invalid frequency
netedit.modifyAdditionalDefaultValue(3, "-30")

# try to create a E2 with different frequency
netedit.leftClick(match, 350, 250)

# set valid frequency
netedit.modifyAdditionalDefaultValue(3, "120")

# create E2 with different frequency
netedit.leftClick(match, 350, 250)

# Change cont
netedit.modifyAdditionalDefaultBoolValue(4)

# create E2 with different cont
netedit.leftClick(match, 450, 250)

# set invalid time threshold
netedit.modifyAdditionalDefaultValue(5, "-12")

# create E2 with different time threshold
netedit.leftClick(match, 150, 220)

# set valid time threshold
netedit.modifyAdditionalDefaultValue(5, "10")

# create E2 with different time threshold
netedit.leftClick(match, 150, 220)

# set invalid speed threshold
netedit.modifyAdditionalDefaultValue(6, "-14")

# try to create E2 with different speed threshold
netedit.leftClick(match, 250, 220)

# set valid speed threshold
netedit.modifyAdditionalDefaultValue(6, "2.5")

# create E2 with different speed threshold
netedit.leftClick(match, 250, 220)

# set invalid jam threshold
netedit.modifyAdditionalDefaultValue(7, "-30")

# try to create E2 with different jam threshold
netedit.leftClick(match, 350, 220)

# set valid jam threshold
netedit.modifyAdditionalDefaultValue(7, "15.5")

# create E2 with different jam threshold
netedit.leftClick(match, 350, 220)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(8)

# create E2 with different friendlyPos
netedit.leftClick(match, 450, 220)

# Check undo redo
netedit.undo(match, 8)
netedit.redo(match, 8)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
