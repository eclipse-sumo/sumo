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

# set delete mode
netedit.deleteMode()

# disable 'Automatically delete additionals'
netedit.protectElements(referencePosition)

# remove two left edges
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftTop)
netedit.leftClick(referencePosition, netedit.positions.network.edge.rightTop)

# Rebuild network
netedit.rebuildNetwork()

# remove two right edges
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftBot)
netedit.leftClick(referencePosition, netedit.positions.network.edge.rightBot)

# Rebuild network
netedit.rebuildNetwork()

# remove two up edges
netedit.leftClick(referencePosition, netedit.positions.network.edge.botLeft)
netedit.leftClick(referencePosition, netedit.positions.network.edge.botRight)

# Rebuild network
netedit.rebuildNetwork()

# remove two down edges
netedit.leftClick(referencePosition, netedit.positions.network.edge.topLeft)
netedit.leftClick(referencePosition, netedit.positions.network.edge.topRight)

# Rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 8)

# Rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 8)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
