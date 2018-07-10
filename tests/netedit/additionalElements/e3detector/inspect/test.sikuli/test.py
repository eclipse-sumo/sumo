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
netedit.leftClick(match, 250, 100)
netedit.leftClick(match, 250, 200)
netedit.leftClick(match, 450, 100)
netedit.leftClick(match, 450, 200)

# select entry detector
netedit.changeAdditional("detExit")

# Create Exit detector E3 (for saving)
netedit.leftClick(match, 250, 100)
netedit.leftClick(match, 250, 450)
netedit.leftClick(match, 450, 100)
netedit.leftClick(match, 450, 450)

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
netedit.modifyAttribute(1, "dummy position")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (different position X)
netedit.modifyAttribute(1, "25, 25")

# Change parameter 2 with a non valid value (non numeral)
netedit.modifyAttribute(2, "dummyFrequency")

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-100")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "120")

# Change parameter 3 with an non valid value
netedit.modifyAttribute(3, "%%%&&%$%$")

# Change parameter 3 with a duplicated value
netedit.modifyAttribute(3, "customName")

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

# Change parameter 8 with a valid value
netedit.modifyBoolAttribute(8)

# Check undos and redos
netedit.undo(match, 17)
netedit.redo(match, 17)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
