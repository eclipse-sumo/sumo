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

# select rerouter
netedit.changeAdditional("rerouter")

# try to create rerouter without edge child (Warning)
netedit.leftClick(referencePosition, 50, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# create rerouter with default parameters
netedit.leftClick(referencePosition, 100, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# set invalid filename
netedit.modifyAdditionalDefaultValue(2, "$$&&$$$$$$")

# try to create rerouter with invalid name
netedit.leftClick(referencePosition, 200, 100)

# set valid name
netedit.modifyAdditionalDefaultValue(2, "custom Name")

# create rerouter with valid name
netedit.leftClick(referencePosition, 200, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# set invalid filename
netedit.modifyAdditionalDefaultValue(3, "$$$$$$$$")

# try to create rerouter with invalid filename
netedit.leftClick(referencePosition, 300, 100)

# set valid filename
netedit.modifyAdditionalDefaultValue(3, "myOwnFilenameRerouter.txt")

# create rerouter with valid filename
netedit.leftClick(referencePosition, 300, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# set invalid probability
netedit.modifyAdditionalDefaultValue(4, "2")

# try to create rerouter with invalid probability
netedit.leftClick(referencePosition, 400, 100)

# set valid probability
netedit.modifyAdditionalDefaultValue(4, "0.3")

# create rerouter with valid probability
netedit.leftClick(referencePosition, 400, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# set invalid time treshold (dummy)
netedit.modifyAdditionalDefaultValue(5, "dummyFrequency")

# try to create rerouter with invalid time treshold
netedit.leftClick(referencePosition, 500, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# set invalid time treshold (negative)
netedit.modifyAdditionalDefaultValue(5, "-3")

# try to create rerouter with invalid time treshold
netedit.leftClick(referencePosition, 500, 100)

# set valid time treshold
netedit.modifyAdditionalDefaultValue(5, "2.7")

# create rerouter with valid time treshold
netedit.leftClick(referencePosition, 500, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# set invalid vtypes
netedit.modifyAdditionalDefaultValue(6, "%%% type2 &&&&")

# create rerouter with valid time treshold
netedit.leftClick(referencePosition, 550, 100)

# set valid time treshold
netedit.modifyAdditionalDefaultValue(6, "type1 type2 type3")

# create rerouter with valid time treshold
netedit.leftClick(referencePosition, 550, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# change off
netedit.modifyAdditionalDefaultBoolValue(7)

# create route with different off
netedit.leftClick(referencePosition, 600, 100)

# select edge child
netedit.selectAdditionalChild(12, 0)

# change block movement
netedit.modifyAdditionalDefaultBoolValue(9)

# try to create rerouter with different timeTreshold
netedit.leftClick(referencePosition, 200, 200)

# Check undo redo
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
