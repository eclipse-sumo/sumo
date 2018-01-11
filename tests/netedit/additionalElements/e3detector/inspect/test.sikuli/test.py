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
netedit.setZoom("25", "25", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(match, 250, 100)

# create E3 2 (for check duplicated ID)
netedit.leftClick(match, 450, 100)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector E3 (for saving)
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 100, 200)
netedit.selectAdditionalChild(6, 1)
netedit.leftClick(match, 400, 300)

# go to inspect mode
netedit.inspectMode()

# inspect first E3
netedit.leftClick(match, 250, 100)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "e3Detector_1")

# Change parameter 0 with a non valid value (Invalid ID)
netedit.modifyAttribute(0, "Id with spaces")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correctID")

# Change parameter 1 with a non valid value (dummy position X)
netedit.modifyAttribute(1, "dummy position X")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (different position X)
netedit.modifyAttribute(1, "25")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummy position Y")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(2, "")

# Change parameter 2 with a non valid value (different position Y)
netedit.modifyAttribute(2, "25")

# Change parameter 3 with a non valid value (non numeral)
netedit.modifyAttribute(3, "dummyFrequency")

# Change parameter 3 with a non valid value (negative)
netedit.modifyAttribute(3, "-100")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "120")

# Change parameter 4 with a non valid value
netedit.modifyAttribute(4, "%%%%%&%&&")

# Change parameter 4 with an empty value
netedit.modifyAttribute(4, "")

# Change parameter 4 with a duplicated value
netedit.modifyAttribute(4, "e3Detector_1.txt")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "myOwnOutput.txt")

# Change parameter 5 with a non valid value (dummy)
netedit.modifyAttribute(5, "dummyTimeTreshold")

# Change parameter 5 with a non valid value (negative)
netedit.modifyAttribute(5, "-5")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "4")

# Change parameter 6 with a non valid value (dummy)
netedit.modifyAttribute(6, "dummySpeedTreshold")

# Change parameter 6 with a non valid value (negative)
netedit.modifyAttribute(6, "-12.1")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "6.3")

# go to a empty area
netedit.leftClick(match, 0, 0)

# Check undos and redos
netedit.undo(match, 21)
netedit.redo(match, 21)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
