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

# select E1
netedit.changeAdditional("e1Detector")

# create E1 1
netedit.leftClick(referencePosition, 250, 150)

# create E1 2
netedit.leftClick(referencePosition, 450, 150)

# go to inspect mode
netedit.inspectMode()

# inspect first E1
netedit.leftClick(referencePosition, 250, 150)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "e1Detector_gneE2_1_1")

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

# Change parameter 3 with a non valid value (non numeral)
netedit.modifyAttribute(3, "dummyFrequency")

# Change parameter 3 with a non valid value (negative)
netedit.modifyAttribute(3, "-100")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "120")

# Change parameter 4 with an non valid value
netedit.modifyAttribute(4, "%%%;:..&&%$%$")

# Change parameter 4 with a duplicated value
netedit.modifyAttribute(4, "customName")

# Change parameter 5 with an empty value
netedit.modifyAttribute(5, "")

# Change parameter 5 with an non valid value
netedit.modifyAttribute(5, "%%%;:..&&%$%$")

# Change parameter 5 with a duplicated value
netedit.modifyAttribute(5, "e1Detector_gneE2_1_1.txt")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "myOwnOutput.txt")

# Change parameter 6 with a empty value
netedit.modifyAdditionalDefaultValue(6, "")

# Change parameter 6 with a invalid value
netedit.modifyAdditionalDefaultValue(6, "Type%%%%%1 T;;yp$2 Type3")

# Change parameter 6 with a valid value
netedit.modifyAdditionalDefaultValue(6, "Type1 Type2 Type3")

# Change boolean parameter 7
netedit.modifyBoolAttribute(7)

# Change boolean parameter 11
netedit.modifyBoolAttribute(11)

# Check undos and redos
netedit.undo(referencePosition, 14)
netedit.redo(referencePosition, 14)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
