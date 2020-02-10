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

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)


# rebuild network
netedit.rebuildNetwork()

# Change to delete mode
netedit.deleteMode()

# remove one way edge
netedit.leftClick(referencePosition, 50, 63)

# remove two way edges
netedit.leftClick(referencePosition, 260, 63)

# remove two way edges
netedit.leftClick(referencePosition, 500, 63)

# remove square
netedit.leftClick(referencePosition, 58, 173)
netedit.leftClick(referencePosition, 150, 280)

# remove circular road
netedit.leftClick(referencePosition, 450, 270)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 6)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 6)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
