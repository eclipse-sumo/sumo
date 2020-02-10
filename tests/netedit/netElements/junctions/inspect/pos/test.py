#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2020 German Aerospace Center (DLR) and others.
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
import time

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(10)

# inspect central node
netedit.leftClick(referencePosition, 325, 250)

# change position with a non valid value
netedit.modifyAttribute(1, "dummy position", False)

# change position with a non valid value (another junction in the same position)
# BUG #
# netedit.modifyAttribute(1, "0.00,50.00")

# avoid merging
# BUG #
# netedit.typeTwoKeys("n", Key.ALT)

# change position with a valid value
netedit.modifyAttribute(1, "40.00,40.00", False)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(10)

# Check undo
netedit.undo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(10)

# Check redo
netedit.redo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(10)

# save additionals
netedit.saveAdditionals(referencePosition)

# Fix stopping places position
netedit.fixStoppingPlace("fixPositions")

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
