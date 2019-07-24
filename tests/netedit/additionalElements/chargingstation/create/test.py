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
netedit.changeElement("chargingStation")

# set name
netedit.changeDefaultValue(2, "chargingStation")

# set friendlyPos
netedit.changeDefaultBoolValue(3)

# set invalid charging power
netedit.changeDefaultValue(4, "-200")

# try to create chargingStation in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# set valid charging power
netedit.changeDefaultValue(4, "12000")

# create chargingStation in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# change reference to right
netedit.changeDefaultValue(9, "reference right")

# set invalid efficiency
netedit.changeDefaultValue(5, "2")

# try create chargingStation in mode "reference right"
netedit.leftClick(referencePosition, 240, 250)

# set valid efficiency
netedit.changeDefaultValue(5, "0.3")

# create chargingStation in mode "reference right"
netedit.leftClick(referencePosition, 240, 250)

# change reference to center
netedit.changeDefaultValue(9, "reference center")

# Change change in transit
netedit.changeDefaultBoolValue(6)

# create chargingStation in mode "reference center"
netedit.leftClick(referencePosition, 425, 250)

# Change length
netedit.changeDefaultValue(11, "30")

# change reference to "reference left"
netedit.changeDefaultValue(9, "reference left")

# set invalid charge delay
netedit.changeDefaultValue(7, "-5")

# try to create a chargingStation in mode "reference left" forcing poisition
netedit.leftClick(referencePosition, 500, 250)

# valid charge delay
netedit.changeDefaultValue(7, "7")

# create a chargingStation in mode "reference left" forcing poisition
netedit.leftClick(referencePosition, 500, 250)

# change reference to "reference right"
netedit.changeDefaultValue(9, "reference right")

# create a chargingStation in mode "reference right"
netedit.leftClick(referencePosition, 110, 250)

# disable friendlyPos
netedit.changeDefaultBoolValue(3)

# change reference to "reference left"
netedit.changeDefaultValue(9, "reference left")

# create a chargingStation in mode "reference left" without friendlyPos
netedit.leftClick(referencePosition, 120, 215)

# change reference to "reference right"
netedit.changeDefaultValue(9, "reference right")

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
