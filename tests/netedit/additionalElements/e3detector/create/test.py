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

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 with default parameters (will not be writed)
netedit.leftClick(referencePosition, 0, 100)

# create E3 with default parameters
netedit.leftClick(referencePosition, 100, 100)

# set invalid frequency
netedit.modifyAdditionalDefaultValue(2, "-30")

# try to create E3 with different frequency
netedit.leftClick(referencePosition, 200, 100)

# set valid frequency
netedit.modifyAdditionalDefaultValue(2, "150")

# create E3 with different frequency
netedit.leftClick(referencePosition, 200, 100)

# set invalid name
netedit.modifyAdditionalDefaultValue(3, "\"\"\"")

# try to create E3 with invalid name
netedit.leftClick(referencePosition, 250, 50)

# set valid name
netedit.modifyAdditionalDefaultValue(3, "customName")

# create E3 with valid name
netedit.leftClick(referencePosition, 250, 50)

# set invalid file
netedit.modifyAdditionalDefaultValue(4, "%%%$$%%$%")

# try to create E3 with invalid file
netedit.leftClick(referencePosition, 300, 100)

# set valid file
netedit.modifyAdditionalDefaultValue(4, "myOwnFile.txt")

# create E3 with different file
netedit.leftClick(referencePosition, 300, 100)

# set invalid vehicle types (invalid IDs)
netedit.modifyAdditionalDefaultValue(5, "%%;$$$ %%$$ type.3")

# try to create E3 with invalid vehicle types
netedit.leftClick(referencePosition, 400, 100)

# set valid vehicle type
netedit.modifyAdditionalDefaultValue(5, "private passenger taxi bus")

# set invalid timeTreshold
netedit.modifyAdditionalDefaultValue(6, "-4")

# try to create E3 with different timeTreshold
netedit.leftClick(referencePosition, 400, 100)

# set valid timeTreshold
netedit.modifyAdditionalDefaultValue(6, "5")

# create E3 with different timeTreshold
netedit.leftClick(referencePosition, 400, 100)

# set invalid speedTreshold
netedit.modifyAdditionalDefaultValue(7, "-3.80")

# try to create E3 with different speedTreshold
netedit.leftClick(referencePosition, 500, 100)

# set valid speedTreshold
netedit.modifyAdditionalDefaultValue(7, "2.51")

# create E3 with different speedTreshold
netedit.leftClick(referencePosition, 500, 100)

# set block movement
netedit.modifyAdditionalDefaultBoolValue(11)

# create E3 with different block movement
netedit.leftClick(referencePosition, 600, 100)

# restore block movement
netedit.modifyAdditionalDefaultBoolValue(11)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry and exit detectors for all E3 detectors (except for the first, that only have one Entry)
netedit.leftClick(referencePosition, 0, 100)  # select E3
netedit.leftClick(referencePosition, 0, 250)

netedit.leftClick(referencePosition, 100, 100)  # select E3
netedit.leftClick(referencePosition, 100, 200)

netedit.leftClick(referencePosition, 200, 100)  # select E3
netedit.leftClick(referencePosition, 200, 250)

netedit.leftClick(referencePosition, 250, 50)  # select E3
netedit.leftClick(referencePosition, 250, 200)

netedit.leftClick(referencePosition, 300, 100)  # select E3
netedit.leftClick(referencePosition, 300, 250)

netedit.leftClick(referencePosition, 400, 100)  # select E3
netedit.leftClick(referencePosition, 400, 200)

netedit.leftClick(referencePosition, 500, 100)  # select E3
netedit.leftClick(referencePosition, 500, 250)

netedit.leftClick(referencePosition, 600, 100)  # select E3
netedit.leftClick(referencePosition, 600, 200)

# select entry detector
netedit.changeAdditional("detExit")

# Create Entry and exit detectors for all E3 detectors (except for the first, that only have one Entry)
netedit.leftClick(referencePosition, 100, 100)  # select E3
netedit.leftClick(referencePosition, 100, 400)

netedit.leftClick(referencePosition, 200, 100)  # select E3
netedit.leftClick(referencePosition, 200, 450)

netedit.leftClick(referencePosition, 250, 50)  # select E3
netedit.leftClick(referencePosition, 250, 400)

netedit.leftClick(referencePosition, 300, 100)  # select E3
netedit.leftClick(referencePosition, 300, 450)

netedit.leftClick(referencePosition, 400, 100)  # select E3
netedit.leftClick(referencePosition, 400, 400)

netedit.leftClick(referencePosition, 500, 100)  # select E3
netedit.leftClick(referencePosition, 500, 450)

netedit.leftClick(referencePosition, 600, 100)  # select E3
netedit.leftClick(referencePosition, 600, 400)

# Check undo redo
netedit.undo(referencePosition, 23)
netedit.redo(referencePosition, 23)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
