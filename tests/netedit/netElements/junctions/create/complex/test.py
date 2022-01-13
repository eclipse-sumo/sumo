#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2022 German Aerospace Center (DLR) and others.
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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--new', '--gui-testing-debug-gl'])

# Change to create edge mode
netedit.createEdgeMode()

# select two-way mode
netedit.changeEditMode('5')

# select chain mode
netedit.changeEditMode('4')

# create a circular road
netedit.leftClick(referencePosition, 167, 135)
netedit.leftClick(referencePosition, 271, 135)
netedit.leftClick(referencePosition, 375, 135)
netedit.leftClick(referencePosition, 375, 241)
netedit.leftClick(referencePosition, 375, 345)
netedit.leftClick(referencePosition, 271, 345)
netedit.leftClick(referencePosition, 167, 345)
netedit.leftClick(referencePosition, 167, 241)
netedit.leftClick(referencePosition, 167, 135)
netedit.cancelEdge()

# disable chain mode
netedit.changeEditMode('4')

# create a complex intersection
netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 167, 135)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 271, 135)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 375, 135)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 375, 241)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 375, 345)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 271, 345)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 167, 345)

netedit.leftClick(referencePosition, 273, 240)
netedit.leftClick(referencePosition, 167, 241)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(referencePosition, 16)
netedit.redo(referencePosition, 16)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
