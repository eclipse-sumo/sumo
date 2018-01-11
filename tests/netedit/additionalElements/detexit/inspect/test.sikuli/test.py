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

# create E3
netedit.leftClick(match, 250, 400)

# create second E3
netedit.leftClick(match, 350, 400)

# select exit detector
netedit.changeAdditional("detExit")

# Create Exit detector E3
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 200, 200)

# go to inspect mode
netedit.inspectMode()

# inspect Exit
netedit.leftClick(match, 200, 200)

# Change parameter 0 with a non valid value (dummy Lane)
netedit.modifyAttribute(0, "dummyLane")

# Change parameter 0 with a non valid value (Empty lane)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a valid value (other lane)
netedit.modifyAttribute(0, "gneE3_1")

# Change parameter 1 with a non valid value (dummy position X)
netedit.modifyAttribute(1, "dummy position")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (different position X)
netedit.modifyAttribute(1, "25")

# Change boolean parameter 2
netedit.modifyBoolAttribute(2)

# Change parameter 1 with a non valid value (Invalid E3 ID)
netedit.modifyAttribute(3, "invalidE3")

# Change parameter 1 with a non valid value (Invalid E3 ID)
netedit.modifyAttribute(3, "e3Detector_1")

# Change boolean parameter 4 (block)
netedit.modifyBoolAttribute(4)

# Check undos and redos
netedit.undo(match, 8)
netedit.redo(match, 8)

# save additionals
netedit.saveAdditionals()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
