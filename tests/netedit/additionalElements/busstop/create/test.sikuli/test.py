#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

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
