#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(3)

# inspect central node
netedit.leftClick(referencePosition, 325, 250)

# change position with a non valid value
netedit.modifyAttribute(netedit.attrs.junction.inspectTLS.pos, "dummy position", False)

# change position with a non valid value (another junction in the same position)
netedit.modifyAttribute(netedit.attrs.junction.inspectTLS.pos, "0.00,50.00", False)

# avoid merging
netedit.typeTwoKeys('alt', 'n')

# rebuild network
netedit.rebuildNetwork()

# inspect central node in new position
netedit.leftClick(referencePosition, 0, 250)

# change position with a valid value
netedit.modifyAttribute(netedit.attrs.junction.inspectTLS.pos, "40.00,40.00", False)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(3)

# Check undo
netedit.undo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(3)

# Check redo
netedit.redo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# wait for output
time.sleep(3)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# Fix stopping places position
netedit.fixStoppingPlace("fixPositions")

# quit netedit
netedit.quit(neteditProcess)
