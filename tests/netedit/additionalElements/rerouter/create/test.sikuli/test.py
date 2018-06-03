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

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select rerouter
netedit.changeAdditional("rerouter")

# try to create rerouter without edge child (Warning)
netedit.leftClick(match, 100, 100)

# select edge child
netedit.selectAdditionalChild(10, 0)

# create rerouter with default parameters
netedit.leftClick(match, 200, 100)

# set invalid filename
netedit.modifyAdditionalDefaultValue(2, "$$$$$$$$")

# try to create rerouter with invalid filename
netedit.leftClick(match, 300, 100)

# set valid filename
netedit.modifyAdditionalDefaultValue(2, "myOwnFilename.txt")

# create rerouter with valid filename
netedit.leftClick(match, 300, 100)

# set invalid probability
netedit.modifyAdditionalDefaultValue(3, "2")

# try to create rerouter with invalid probability
netedit.leftClick(match, 400, 100)

# set valid probability
netedit.modifyAdditionalDefaultValue(3, "0.3")

# create rerouter with valid probability
netedit.leftClick(match, 400, 100)

# set invalid frequency (dummy)
netedit.modifyAdditionalDefaultValue(4, "dummyFrequency")

# create rerouter with invalid frequency
netedit.leftClick(match, 500, 100)

# set invalid frequency (negative)
netedit.modifyAdditionalDefaultValue(4, "-3")

# create rerouter with invalid frequency
netedit.leftClick(match, 500, 100)

# set valid frequency
netedit.modifyAdditionalDefaultValue(4, "2.7")

# create rerouter with invalid frequency
netedit.leftClick(match, 500, 100)

# change off
netedit.modifyAdditionalDefaultBoolValue(5)

# try to create rerouter with different timeTreshold
netedit.leftClick(match, 600, 100)

# Check undo redo
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
