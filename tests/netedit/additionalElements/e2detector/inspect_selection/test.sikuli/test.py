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

# inspect E2s
netedit.leftClick(referencePosition, 150, 250)

# Set invalid length
netedit.modifyAttribute(0, "-12")

# Set valid length
netedit.modifyAttribute(0, "7")

# Set invalid freq
netedit.modifyAttribute(1, "-30")

# Set valid freq
netedit.modifyAttribute(1, "50")

# set invalid name
netedit.modifyAttribute(2, "%%%;:..&&%$%$")

# set valid name
netedit.modifyAttribute(2, "customName")

# set invalid filename
netedit.modifyAttribute(3, "&&&&&&&&")

# set valid filename
netedit.modifyAttribute(3, "myOwnFilename.txt")

# Change parameter 4 with a invalid value
netedit.modifyAdditionalDefaultValue(4, "Type%%%%%1 T;;yp$2 Type3")

# Change parameter 4 with a valid value
netedit.modifyAdditionalDefaultValue(4, "Type1 Type2 Type3")

# Set invalid time speed treshold
netedit.modifyAttribute(5, "-5")

# Set valid time speed treshold
netedit.modifyAttribute(5, "11.3")

# Set invalid speed speed treshold
netedit.modifyAttribute(6, "-3")

# Set valid speed speed treshold
netedit.modifyAttribute(6, "4")

# Set invalid jam speed treshold
netedit.modifyAttribute(7, "-6")

# Set valid jam speed treshold
netedit.modifyAttribute(7, "8.5")

# Set friendlyPos
netedit.modifyBoolAttribute(8)

# Set block movement
netedit.modifyBoolAttribute(12)

# Check undo redo
netedit.undo(referencePosition, 10)
netedit.redo(referencePosition, 10)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
