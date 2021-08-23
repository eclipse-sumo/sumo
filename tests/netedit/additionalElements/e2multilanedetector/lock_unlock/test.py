#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

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

# recompute
netedit.rebuildNetwork()

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeElement("e2MultilaneDetector")

# create E2 with default parameters
netedit.leftClick(referencePosition, 190, 240)
netedit.leftClick(referencePosition, 440, 240)
netedit.typeEnter()

# go to inspect mode
netedit.inspectMode()

# inspect E2
netedit.leftClick(referencePosition, 320, 240)

# Change boolean parameter block move
netedit.modifyBoolAttribute(15, True)

# go to move mode
netedit.moveMode()

# try to move E2 to left taking the first lane
netedit.moveElement(referencePosition, 200, 255, 20, 240)

# go to inspect mode
netedit.inspectMode()

# inspect E2
netedit.leftClick(referencePosition, 320, 240)

# Change boolean parameter block move
netedit.modifyBoolAttribute(15, True)

# go to move mode
netedit.moveMode()

# move E2 to left taking the first lane
netedit.moveElement(referencePosition, 200, 255, 20, 240)

# Check undo redo
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# apply zoom
netedit.setZoom("25", "0", "74")

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
