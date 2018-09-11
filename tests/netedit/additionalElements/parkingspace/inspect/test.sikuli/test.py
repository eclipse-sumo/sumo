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

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeAdditional("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 180)

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 400, 180)

# select space
netedit.changeAdditional("space")

# create first space
netedit.selectAdditionalChild(9, 0)
netedit.leftClick(referencePosition, 250, 320)

# create second space
netedit.selectAdditionalChild(9, 0)
netedit.leftClick(referencePosition, 400, 320)

# go to inspect mode
netedit.inspectMode()

# inspect space
netedit.leftClick(referencePosition, 250, 300)

# Change parameter 0 with a non valid value (dummy)
netedit.modifyAttribute(0, "dummyX")

# Change parameter 0 with a non valid value (empty)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a valid value (-negative)
netedit.modifyAttribute(0, "-3.5")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "3")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyY")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (-negative)
netedit.modifyAttribute(1, "-3")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "2.5")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummyZ")

# Change parameter 2 with a non valid value (empty)
netedit.modifyAttribute(2, "")

# Change parameter 2 with a valid value (-negative)
netedit.modifyAttribute(2, "-4.25")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "1.00")

# Change parameter 7 with a non valid value (dummy)
netedit.modifyAttribute(9, "dummyParent")

# Change parameter 7 with a non valid value (empty)
netedit.modifyAttribute(9, "")

# Change parameter 7 with a valid value
netedit.modifyAttribute(9, "parkingArea_gneE2_0_1")

# Change parameter 7 with a different value
netedit.modifyBoolAttribute(10)

# inspect the other space
netedit.leftClick(referencePosition, 400, 300)

# Change parameter 3 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyWidth")

# Change parameter 3 with a non valid value (negative)
netedit.modifyAttribute(3, "-5")

# Change parameter 3 with a non valid value (0)
netedit.modifyAttribute(3, "0")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "2.3")

# Change parameter 4 with a non valid value (dummy)
netedit.modifyAttribute(4, "dummyHeight")

# Change parameter 4 with a non valid value (negative)
netedit.modifyAttribute(4, "-6")

# Change parameter 4 with a non valid value (0)
netedit.modifyAttribute(4, "0")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "3.4")

# Change parameter 5 with a non valid value (dummy)
netedit.modifyAttribute(5, "dummyAngle")

# Change parameter 5 with a valid value (negative)
netedit.modifyAttribute(5, "-6")

# Change parameter 5 with a valid value >360
netedit.modifyAttribute(5, "500")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "32.5")

# Check undos and redos
netedit.undo(referencePosition, 24)
netedit.redo(referencePosition, 24)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
