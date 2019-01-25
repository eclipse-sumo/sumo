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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# set invalid vehicle types (invalid IDs)
netedit.modifyAdditionalDefaultValue(5, "@%%;$$$ %%$$ type.3@")

# try to create E3 with invalid vehicle type
netedit.leftClick(referencePosition, 100, 100)

# set valid vehicle types
netedit.modifyAdditionalDefaultValue(5, "type1 type2 type3")

# create E3 with valid vehicle types
netedit.leftClick(referencePosition, 100, 100)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry and exit detectors for all E3 detectors (except for the first, that only have one Entry)
netedit.leftClick(referencePosition, 100, 100)  # select E3
netedit.leftClick(referencePosition, 300, 250)

# select entry detector
netedit.changeAdditional("detExit")

# Create Entry and exit detectors for all E3 detectors (except for the first, that only have one Entry)
netedit.leftClick(referencePosition, 100, 100)  # select E3
netedit.leftClick(referencePosition, 300, 400)

# Check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
