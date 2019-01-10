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

# inspect E3s
netedit.leftClick(referencePosition, 150, 250)

# Set invalid freq
netedit.modifyAttribute(0, "-30")

# Set valid freq
netedit.modifyAttribute(0, "50")

# Change parameter 1 with an non valid value
netedit.modifyAttribute(1, "%%%;:..&&%$%$")

# Change parameter 1 with a duplicated value
netedit.modifyAttribute(1, "customName")

# set invalid filename
netedit.modifyAttribute(2, "&&&&&&&&")

# set valid filename
netedit.modifyAttribute(2, "myOwnFilename.txt")

# Change parameter 3 with a invalid value
netedit.modifyAdditionalDefaultValue(3, "Type%%%%%1 T;;yp$2 Type3")

# Change parameter 3 with a valid value
netedit.modifyAdditionalDefaultValue(3, "Type1 Type2 Type3")

# Set invalid time speed treshold
netedit.modifyAttribute(4, "-5")

# Set valid time speed treshold
netedit.modifyAttribute(4, "11.3")

# Set invalid speed speed treshold
netedit.modifyAttribute(5, "-3")

# Set valid speed speed treshold
netedit.modifyAttribute(5, "4")

# change block movement
netedit.modifyBoolAttribute(9)

# Check undo redo
netedit.undo(referencePosition, 9)
netedit.redo(referencePosition, 9)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
