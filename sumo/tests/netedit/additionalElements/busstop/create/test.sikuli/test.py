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

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeAdditional("busStop")

# set name
netedit.modifyAdditionalDefaultValue(2, "busStop")

# set friendlyPos
netedit.modifyAdditionalDefaultBoolValue(3)

# Add three extra lines
netedit.modifyStoppingPlaceLines(5, 3)

# fill extra lines
netedit.fillStoppingPlaceLines(4, 4)

# remove last line (line 4)
netedit.modifyStoppingPlaceLines(9, 1)

# create busStop in mode "reference left"
netedit.leftClick(match, 250, 250)

# change reference to right
netedit.modifyAdditionalDefaultValue(10, "reference right")

# create busStop in mode "reference right"
netedit.leftClick(match, 240, 250)

# change reference to center
netedit.modifyAdditionalDefaultValue(10, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(match, 425, 250)

# Change length
netedit.modifyAdditionalDefaultValue(12, "30")

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(10, "reference left")

# create a busStop in mode "reference left"
netedit.leftClick(match, 500, 250)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(10, "reference right")

# create a busStop in mode "reference right"
netedit.leftClick(match, 110, 250)

# disable friendlyPos
netedit.modifyAdditionalDefaultBoolValue(3)

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(10, "reference left")

# create a busStop in mode "reference left" without friendlyPos
netedit.leftClick(match, 120, 215)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(10, "reference right")

# create a busStop in mode "reference right" without friendlyPos
netedit.leftClick(match, 500, 215)

# Check undo redo
netedit.undo(match, 7)
netedit.redo(match, 7)

# save additionals
netedit.saveAdditionals()

# Fix stopping places position
netedit.fixStoppingPlace("fixPositions")

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
