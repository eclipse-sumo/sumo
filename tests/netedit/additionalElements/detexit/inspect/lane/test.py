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

# create E3
netedit.leftClick(referencePosition, 250, 100)

# select Entry detector
netedit.changeAdditional("detExit")

# Create Entry detector E3
netedit.leftClick(referencePosition, 250, 100)
netedit.leftClick(referencePosition, 180, 200)

# select Exit detector and create it
netedit.changeAdditional("detEntry")
netedit.leftClick(referencePosition, 250, 100)
netedit.leftClick(referencePosition, 400, 200)

# go to inspect mode
netedit.inspectMode()

# inspect Entry
netedit.leftClick(referencePosition, 200, 200)

# Change parameter lane with a non valid value (dummy Lane)
netedit.modifyAttribute(3, "dummyLane")

# Change parameter lane with a non valid value (Empty lane)
netedit.modifyAttribute(3, "")

# Change parameter lane with a valid value (other lane)
netedit.modifyAttribute(3, "gneE3_1")

# Check undos and redos
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
