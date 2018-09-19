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

# select E1 Instant
netedit.changeAdditional("instantInductionLoop")

# create E1 Instant with default parameters
netedit.leftClick(referencePosition, 150, 250)

# set invalid name
netedit.modifyAdditionalDefaultValue(2, "\"\"\"")

# try to create E1 with invalid name
netedit.leftClick(referencePosition, 200, 250)

# set valid name
netedit.modifyAdditionalDefaultValue(2, "customName")

# try to create E1 with invalid name
netedit.leftClick(referencePosition, 200, 250)

# set invalid filename
netedit.modifyAdditionalDefaultValue(3, "&&&&&&&&")

# try to create E1 Instant with invalid filename
netedit.leftClick(referencePosition, 300, 250)

# set valid filename
netedit.modifyAdditionalDefaultValue(3, "myOwnFilename.txt")

# create E1 Instant with valid filename
netedit.leftClick(referencePosition, 300, 250)

# set invalid vehicle types (invalid IDs)
netedit.modifyAdditionalDefaultValue(4, "%%;$$$ %%$$ type.3")

# try to create E1 with invalid vehicle types
netedit.leftClick(referencePosition, 400, 250)

# set valid vehicle type
netedit.modifyAdditionalDefaultValue(4, "private passenger taxi bus")

# create E1 with valid vehicle types
netedit.leftClick(referencePosition, 400, 250)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(5)

# create E1 Instant with different friendly pos
netedit.leftClick(referencePosition, 450, 250)

# Change block move
netedit.modifyAdditionalDefaultBoolValue(7)

# create E1 Instant with different block move
netedit.leftClick(referencePosition, 500, 250)

# Check undo redo
netedit.undo(referencePosition, 7)
netedit.redo(referencePosition, 7)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
