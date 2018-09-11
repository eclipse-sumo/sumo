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
netedit.setZoom("25", "20", "25")

# go to additional mode
netedit.additionalMode()

# select BusStop
netedit.changeAdditional("busStop")

# create BusStop with default parameters
netedit.leftClick(referencePosition, 375, 250)

# select Access detector
netedit.changeAdditional("access")

# Create Access detector
netedit.selectAdditionalChild(7, 0)
netedit.leftClick(referencePosition, 200, 50)

# Try to create another Access in the same edge
netedit.selectAdditionalChild(7, 0)
netedit.leftClick(referencePosition, 200, 250)

# go to inspect mode
netedit.inspectMode()

# inspect Access
netedit.leftClick(referencePosition, 208, 260)

# Change parameter 0 with a non valid value (dummy Lane)
netedit.modifyAttribute(0, "dummyLane")

# Change parameter 0 with a non valid value (Empty lane)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a non valid value (There is another Access)
netedit.modifyAttribute(0, "gneE3_0")

# Change parameter 0 with a valid value (other lane)
netedit.modifyAttribute(0, "gneE1_0")

# Change parameter 1 with a non valid value (dummy position X)
netedit.modifyAttribute(1, "dummy position")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (different position X)
netedit.modifyAttribute(1, "25")

# Change lenght (invalid)
netedit.modifyAttribute(2, "-30")

netedit.modifyAttribute(2, "5")

# Change friendly position
netedit.modifyBoolAttribute(3)

# Change block movement
netedit.modifyBoolAttribute(7)

# Check undo redo
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
