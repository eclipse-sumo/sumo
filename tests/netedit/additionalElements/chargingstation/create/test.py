#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
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

# select chargingStation
netedit.changeAdditional("chargingStation")

# set name
netedit.modifyAdditionalDefaultValue(2, "chargingStation")

# set friendlyPos
netedit.modifyAdditionalDefaultBoolValue(3)

# set invalid charging power
netedit.modifyAdditionalDefaultValue(4, "-200")

# try to create chargingStation in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# set valid charging power
netedit.modifyAdditionalDefaultValue(4, "12000")

# create chargingStation in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# change reference to right
netedit.modifyAdditionalDefaultValue(9, "reference right")

# set invalid efficiency
netedit.modifyAdditionalDefaultValue(5, "2")

# try create chargingStation in mode "reference right"
netedit.leftClick(referencePosition, 240, 250)

# set valid efficiency
netedit.modifyAdditionalDefaultValue(5, "0.3")

# create chargingStation in mode "reference right"
netedit.leftClick(referencePosition, 240, 250)

# change reference to center
netedit.modifyAdditionalDefaultValue(9, "reference center")

# Change change in transit
netedit.modifyAdditionalDefaultBoolValue(6)

# create chargingStation in mode "reference center"
netedit.leftClick(referencePosition, 425, 250)

# Change length
netedit.modifyAdditionalDefaultValue(11, "30")

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(9, "reference left")

# set invalid charge delay
netedit.modifyAdditionalDefaultValue(7, "-5")

# try to create a chargingStation in mode "reference left" forcing poisition
netedit.leftClick(referencePosition, 500, 250)

# valid charge delay
netedit.modifyAdditionalDefaultValue(7, "7")

# create a chargingStation in mode "reference left" forcing poisition
netedit.leftClick(referencePosition, 500, 250)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(9, "reference right")

# create a chargingStation in mode "reference right"
netedit.leftClick(referencePosition, 110, 250)

# disable friendlyPos
netedit.modifyAdditionalDefaultBoolValue(3)

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(9, "reference left")

# create a chargingStation in mode "reference left" without friendlyPos
netedit.leftClick(referencePosition, 120, 215)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(9, "reference right")

# create a chargingStation in mode "reference right" without friendlyPos
netedit.leftClick(referencePosition, 500, 215)

# Check undo redo
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# save additionals
netedit.saveAdditionals()

# Fix stopping places position
netedit.fixStoppingPlace("fixPositions")

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
