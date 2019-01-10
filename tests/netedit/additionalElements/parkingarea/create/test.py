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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeAdditional("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 220, 250)

# set name
netedit.modifyAdditionalDefaultValue(2, "&&>>><<<")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 240, 250)

# set name
netedit.modifyAdditionalDefaultValue(2, "parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 240, 250)

# set invalid roadSideCapacity (dummy)
netedit.modifyAdditionalDefaultValue(3, "dummyRoadSideCapacity")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 260, 250)

# set invalid roadSideCapacity (empty"
netedit.modifyAdditionalDefaultValue(3, "")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 260, 250)

# set invalid roadSideCapacity (negative)
netedit.modifyAdditionalDefaultValue(3, "-3")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 260, 250)

# set invalid roadSideCapacity (double)
netedit.modifyAdditionalDefaultValue(3, "2.5")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 260, 250)

# set valid roadSideCapacity (int)
netedit.modifyAdditionalDefaultValue(3, "3")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 260, 250)

# set friendlyPos
netedit.modifyAdditionalDefaultBoolValue(4)

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 270, 250)

# set invalid width (dummy)
netedit.modifyAdditionalDefaultValue(5, "dummyWidth")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 290, 250)

# set invalid width (empty)
netedit.modifyAdditionalDefaultValue(5, "")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 290, 250)

# set invalid width (negative)
netedit.modifyAdditionalDefaultValue(5, "-3")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 290, 250)

# set valid width
netedit.modifyAdditionalDefaultValue(5, "2.5")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 290, 250)

# set invalid height (dummy)
netedit.modifyAdditionalDefaultValue(6, "dummyHeight")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 300, 250)

# set invalid height (empty)
netedit.modifyAdditionalDefaultValue(6, "")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 300, 250)

# set invalid height (negative)
netedit.modifyAdditionalDefaultValue(6, "-4")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 300, 250)

# set valid height
netedit.modifyAdditionalDefaultValue(6, "3.1")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 300, 250)

# set invalid angle (dummy)
netedit.modifyAdditionalDefaultValue(7, "dummyHeight")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 310, 250)

# set invalid angle (empty)
netedit.modifyAdditionalDefaultValue(7, "")

# try to create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 310, 250)

# set valid angle (negative)
netedit.modifyAdditionalDefaultValue(7, "-4")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 310, 250)

# set valid angle (>360)
netedit.modifyAdditionalDefaultValue(7, "500")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 320, 250)

# set valid angle
netedit.modifyAdditionalDefaultValue(7, "120")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 330, 250)

# change reference to right
netedit.modifyAdditionalDefaultValue(9, "reference right")

# create parkingArea in mode "reference right"
netedit.leftClick(referencePosition, 240, 250)

# change reference to center
netedit.modifyAdditionalDefaultValue(9, "reference center")

# create parkingArea in mode "reference center"
netedit.leftClick(referencePosition, 425, 250)

# Change length
netedit.modifyAdditionalDefaultValue(11, "30")

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(9, "reference left")

# create a parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 500, 250)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(9, "reference right")

# create a parkingArea in mode "reference right"
netedit.leftClick(referencePosition, 110, 250)

# disable friendlyPos
netedit.modifyAdditionalDefaultBoolValue(3)

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(9, "reference left")

# create a parkingArea in mode "reference left" without friendlyPos
netedit.leftClick(referencePosition, 120, 215)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(9, "reference right")

# create a parkingArea in mode "reference right" without friendlyPos
netedit.leftClick(referencePosition, 500, 215)

# Check undo redo
netedit.undo(referencePosition, 15)
netedit.redo(referencePosition, 15)

# save additionals
netedit.saveAdditionals()

# Fix stopping places position
netedit.fixStoppingPlace("fixPositions")

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
