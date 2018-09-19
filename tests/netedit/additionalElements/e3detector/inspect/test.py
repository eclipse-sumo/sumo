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
netedit.setZoom("25", "25", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(referencePosition, 250, 100)

# create E3 2 (for check duplicated ID)
netedit.leftClick(referencePosition, 450, 100)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector E3 (for saving)
netedit.leftClick(referencePosition, 250, 100)
netedit.leftClick(referencePosition, 250, 200)
netedit.leftClick(referencePosition, 450, 100)
netedit.leftClick(referencePosition, 450, 200)

# select entry detector
netedit.changeAdditional("detExit")

# Create Exit detector E3 (for saving)
netedit.leftClick(referencePosition, 250, 100)
netedit.leftClick(referencePosition, 250, 450)
netedit.leftClick(referencePosition, 450, 100)
netedit.leftClick(referencePosition, 450, 450)

# go to inspect mode
netedit.inspectMode()

# inspect first E3
netedit.leftClick(referencePosition, 250, 100)

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
netedit.modifyAttribute(3, "%%%;:..&&%$%$")

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

# Change parameter 5 with a invalid value
netedit.modifyAdditionalDefaultValue(5, "Type%%%%%1 T;;yp$2 Type3")

# Change parameter 5 with a valid value
netedit.modifyAdditionalDefaultValue(5, "Type1 Type2 Type3")

# Change parameter 6 with a non valid value (dummy)
netedit.modifyAttribute(6, "dummyTimeTreshold")

# Change parameter 6 with a non valid value (negative)
netedit.modifyAttribute(6, "-5")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "4")

# Change parameter 7 with a non valid value (negative)
netedit.modifyAttribute(7, "-12.1")

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "6.3")

# Change parameter block movement with a valid value
netedit.modifyBoolAttribute(11)

# Check undos and redos
netedit.undo(referencePosition, 18)
netedit.redo(referencePosition, 18)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
