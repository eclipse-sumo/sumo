#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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

# Rebuild network
netedit.rebuildNetwork()

# set crossing mode
netedit.crossingMode()

# select central node
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center)

# select two left edges and create crossing in edges 3 and 7
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftTop)
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftBot)
netedit.createCrossing(True)
netedit.rebuildNetwork()

# Check undo redo
netedit.undo(referencePosition, 1)

netedit.rebuildNetwork()

netedit.redo(referencePosition, 1)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# press space to fix crossings
netedit.typeSpace()

# quit netedit
netedit.quit(neteditProcess)
