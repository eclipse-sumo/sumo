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

# select parkingArea
netedit.changeAdditional("parkingArea")

# change reference to center
netedit.modifyAdditionalDefaultValue(9, "reference center")

# create parkingArea 1 in mode "reference center"
netedit.leftClick(referencePosition, 250, 150)

# create parkingArea 2 in mode "reference center"
netedit.leftClick(referencePosition, 450, 150)

# go to inspect mode
netedit.inspectMode()

# inspect first parkingArea
netedit.leftClick(referencePosition, 250, 200)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "parkingArea_gneE2_1_1")

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

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummyStartPos")

# Change parameter 2 with a valid value (< 0)
netedit.modifyAttribute(2, "-5")

# Change parameter 2 with a non valid value (> endPos)
netedit.modifyAttribute(2, "400")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "20")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyEndPos")

# Change parameter 3 with a valid value (out of range)
netedit.modifyAttribute(3, "3000")

# Change parameter 3 with a non valid value (<startPos)
netedit.modifyAttribute(3, "10")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "30")

# Change parameter 4 with a non valid value (throw warning)
netedit.modifyAttribute(4, "%%%$$$$%$")

# Change parameter 4 with a non valid value (throw warning)
netedit.modifyAttribute(4, "new Parking Area Name")

# Change parameter 45with a non valid value (dummy)
netedit.modifyAttribute(5, "dummyRoadSideCapacity")

# Change parameter 5 with a non valid value (double)
netedit.modifyAttribute(5, "2.3")

# Change parameter 5 with a non valid value (negative)
netedit.modifyAttribute(5, "-5")

# Change parameter 5 with a non valid value (negative)
netedit.modifyAttribute(5, "7")

# Change parameter 6 with a different value
netedit.modifyBoolAttribute(6)

# Change parameter 7 with a non valid value (dummy)
netedit.modifyAttribute(7, "dummyWidth")

# Change parameter 7 with a non valid value (negative)
netedit.modifyAttribute(7, "-5")

# Change parameter 7 with a non valid value (0)
netedit.modifyAttribute(7, "0")

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "2.3")

# Change parameter 8 with a non valid value (dummy)
netedit.modifyAttribute(8, "dummyHeight")

# Change parameter 8 with a non valid value (negative)
netedit.modifyAttribute(8, "-6")

# Change parameter 8 with a non valid value (0)
netedit.modifyAttribute(8, "0")

# Change parameter 8 with a valid value
netedit.modifyAttribute(8, "3.4")

# Change parameter 9 with a non valid value (dummy)
netedit.modifyAttribute(9, "dummyAngle")

# Change parameter 9 with a valid value (negative)
netedit.modifyAttribute(9, "-6")

# Change parameter 9 with a valid value >360
netedit.modifyAttribute(9, "500")

# Change parameter 9 with a valid value
netedit.modifyAttribute(9, "32.5")

# Change parameter 5 with a different value
netedit.modifyBoolAttribute(13)

# Leave start and end positions empty
netedit.modifyAttribute(2, "")
netedit.modifyAttribute(3, "")

# Check undos and redos
netedit.undo(referencePosition, 26)
netedit.redo(referencePosition, 26)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
