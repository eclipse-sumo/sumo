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

# select E2
netedit.changeAdditional("e2Detector")

# create E2 with default parameters
netedit.leftClick(referencePosition, 125, 250)

# set invalid  lenght
netedit.modifyAdditionalDefaultValue(2, "-12")

# try to create E2 with different lenght
netedit.leftClick(referencePosition, 250, 250)

# set valid lenght
netedit.modifyAdditionalDefaultValue(2, "5")

# create E2 with different lenght
netedit.leftClick(referencePosition, 250, 250)

# set invalid frequency
netedit.modifyAdditionalDefaultValue(3, "-30")

# try to create a E2 with different frequency
netedit.leftClick(referencePosition, 350, 250)

# set valid frequency
netedit.modifyAdditionalDefaultValue(3, "120")

# create E2 with different frequency
netedit.leftClick(referencePosition, 350, 250)

# set invalid name
netedit.modifyAdditionalDefaultValue(4, "\"\"\"")

# try to create E1 with invalid name
netedit.leftClick(referencePosition, 500, 250)

# set valid name
netedit.modifyAdditionalDefaultValue(4, "customName")

# create E1 with valid name
netedit.leftClick(referencePosition, 500, 250)

# set invalid filename
netedit.modifyAdditionalDefaultValue(5, "&&&&&&&&")

# try to create E2 with invalid filename
netedit.leftClick(referencePosition, 400, 250)

# set valid filename
netedit.modifyAdditionalDefaultValue(5, "myOwnFilename.txt")

# create E2 with valid filename
netedit.leftClick(referencePosition, 400, 250)

# set invalid vehicle types (invalid IDs)
netedit.modifyAdditionalDefaultValue(6, "%%;$$$ %%$$ type.3")

# try to create E1 with invalid vehicle types
netedit.leftClick(referencePosition, 150, 220)

# set valid vehicle type
netedit.modifyAdditionalDefaultValue(6, "private passenger taxi bus")

# create E1 with valid vehicle types
netedit.leftClick(referencePosition, 150, 220)

# set invalid time threshold
netedit.modifyAdditionalDefaultValue(7, "-12")

# create E2 with different time threshold
netedit.leftClick(referencePosition, 250, 220)

# set valid time threshold
netedit.modifyAdditionalDefaultValue(7, "10")

# create E2 with different time threshold
netedit.leftClick(referencePosition, 250, 220)

# set invalid speed threshold
netedit.modifyAdditionalDefaultValue(8, "-14")

# try to create E2 with different speed threshold
netedit.leftClick(referencePosition, 350, 220)

# set valid speed threshold
netedit.modifyAdditionalDefaultValue(8, "2.5")

# create E2 with different speed threshold
netedit.leftClick(referencePosition, 350, 220)

# set invalid jam threshold
netedit.modifyAdditionalDefaultValue(9, "-30")

# try to create E2 with different jam threshold
netedit.leftClick(referencePosition, 450, 220)

# set valid jam threshold
netedit.modifyAdditionalDefaultValue(9, "15.5")

# create E2 with different jam threshold
netedit.leftClick(referencePosition, 450, 220)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(10)

# create E2 with different friendlyPos
netedit.leftClick(referencePosition, 500, 220)

# Change block movement
netedit.modifyAdditionalDefaultBoolValue(12)

# create E2 with different friendlyPos
netedit.leftClick(referencePosition, 525, 220)

# Check undo redo
netedit.undo(referencePosition, 12)
netedit.redo(referencePosition, 12)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
