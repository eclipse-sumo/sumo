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

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeAdditional("e2Detector")

# create E2 1
netedit.leftClick(match, 250, 150)

# create E2 2
netedit.leftClick(match, 450, 150)

# go to inspect mode
netedit.inspectMode()

# inspect first E2
netedit.leftClick(match, 260, 150)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "e2Detector_gneE2_1_1")

# Change parameter 0 with a non valid value (Invalid ID)
netedit.modifyAttribute(0, "Id with spaces")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correctID")

# Change parameter 1 with a non valid value (dummy lane)
netedit.modifyAttribute(1, "dummy lane")

# Change parameter 1 with a valid value (different edge)
netedit.modifyAttribute(1, "gneE0_0")

# Change parameter 1 with a valid value (original edge, same lane)
netedit.modifyAttribute(1, "gneE2_1")

# Change parameter 1 with a valid value (original edge, different lane)
netedit.modifyAttribute(1, "gneE2_0")

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-5")

# Change parameter 2 with a non valid value (> endPos)
netedit.modifyAttribute(2, "400")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "20")

# Change parameter 3 with a non valid value
netedit.modifyAttribute(3, "dummyLength")

# Change parameter 3 with a non valid value
netedit.modifyAttribute(3, "-5")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "5")

# Change parameter 4 with a non valid value (non numeral)
netedit.modifyAttribute(4, "dummyFrequency")

# Change parameter 4 with a non valid value (negative)
netedit.modifyAttribute(4, "-100")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "120")

# Change parameter 5 with an non valid value
netedit.modifyAttribute(5, "%%%&%%%&&")

# Change parameter 5 with an empty value
netedit.modifyAttribute(5, "")

# Change parameter 5 with a duplicated value
netedit.modifyAttribute(5, "E2Detector_gneE2_1_1.txt")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "myOwnOutput.txt")

# Change boolean parameter 6
netedit.modifyBoolAttribute(6)

# Change parameter 7 with a non valid value
netedit.modifyAttribute(7, "dummyTimeTreshold")

# Change parameter 7 with a non valid value
netedit.modifyAttribute(7, "-5")

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "4")

# Change parameter 8 with a non valid value
netedit.modifyAttribute(8, "dummySpeedTreshold")

# Change parameter 8 with a non valid value
netedit.modifyAttribute(8, "-12.1")

# Change parameter 8 with a valid value
netedit.modifyAttribute(8, "6.3")

# Change parameter 9 with a non valid value
netedit.modifyAttribute(9, "dummyJamThreshold")

# Change parameter 9 with a non valid value
netedit.modifyAttribute(9, "-8")

# Change parameter 9 with a valid value
netedit.modifyAttribute(9, "12.5")

# Change boolean parameter 10
netedit.modifyBoolAttribute(10)

# Check undos and redos
netedit.undo(match, 24)
netedit.redo(match, 24)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
