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

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect E1
netedit.leftClick(referencePosition, 150, 240)

# Change parameter 0 with a non valid value (non numeral)
netedit.modifyAttribute(0, "dummyFrequency")

# Change parameter 0 with a non valid value (negative)
netedit.modifyAttribute(0, "-100")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "120")

# Change parameter 1 with a empty value
netedit.modifyAdditionalDefaultValue(1, "")

# Change parameter 1 with an non valid value
netedit.modifyAttribute(1, "%%%;:..&&%$%$")

# Change parameter 1 with a duplicated value
netedit.modifyAttribute(1, "customName")

# Change parameter 2 with an empty value
netedit.modifyAttribute(2, "")

# Change parameter 2 with an non valid value
netedit.modifyAttribute(2, "%%%;:..&&%$%$")

# Change parameter 2 with a duplicated value
netedit.modifyAttribute(2, "e1Detector_gneE2_1_1.txt")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "myOwnOutput.txt")

# Change parameter 3 with a empty value
netedit.modifyAdditionalDefaultValue(3, "")

# Change parameter 3 with a invalid value
netedit.modifyAdditionalDefaultValue(3, "Type%%%%%1 T;;yp$2 Type3")

# Change parameter 3 with a valid value
netedit.modifyAdditionalDefaultValue(3, "Type1 Type2 Type3")

# Change boolean parameter 4
netedit.modifyBoolAttribute(4)

# Change boolean parameter 6
netedit.modifyBoolAttribute(8)

# Check undos and redos
netedit.undo(referencePosition, 6)
netedit.redo(referencePosition, 6)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
