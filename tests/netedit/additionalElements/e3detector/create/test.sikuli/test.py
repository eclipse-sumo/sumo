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

# select E3
netedit.changeAdditional("e3Detector")

# create E3 with default parameters (will not be writed)
netedit.leftClick(match, 100, 100)

# create E3 with default parameters
netedit.leftClick(match, 200, 100)

# set invalid frequency
netedit.modifyAdditionalDefaultValue(2, "-30")

# try to create E3 with different frequency
netedit.leftClick(match, 300, 100)

# set valid frequency
netedit.modifyAdditionalDefaultValue(2, "150")

# create E3 with different frequency
netedit.leftClick(match, 300, 100)

# set invalid timeTreshold
netedit.modifyAdditionalDefaultValue(3, "-4")

# try to create E3 with different timeTreshold
netedit.leftClick(match, 400, 100)

# set valid timeTreshold
netedit.modifyAdditionalDefaultValue(3, "5")

# create E3 with different timeTreshold
netedit.leftClick(match, 400, 100)

# set invalid speedTreshold
netedit.modifyAdditionalDefaultValue(4, "-3.80")

# try to create E3 with different speedTreshold
netedit.leftClick(match, 500, 100)

# set valid speedTreshold
netedit.modifyAdditionalDefaultValue(4, "2.51")

# create E3 with different speedTreshold
netedit.leftClick(match, 500, 100)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detectors for all E3 detectors except for the first
netedit.selectAdditionalChild(6, 1)
netedit.leftClick(match, 50, 200)
netedit.selectAdditionalChild(6, 1)
netedit.leftClick(match, 200, 250)
netedit.selectAdditionalChild(6, 1)
netedit.leftClick(match, 350, 200)
netedit.selectAdditionalChild(6, 1)
netedit.leftClick(match, 500, 250)

# Check undo redo
netedit.undo(match, 9)
netedit.redo(match, 9)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
