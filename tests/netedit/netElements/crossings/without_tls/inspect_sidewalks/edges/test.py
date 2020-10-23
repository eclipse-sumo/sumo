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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.setZoom("50", "50", "50")

# go to inspect mode
netedit.inspectMode()

# inspect first crossing
netedit.leftClick(referencePosition, 250, 225)

# set invalid edge
netedit.modifyAttribute(0, "dummy Edges", True)

# Change Edges adding a new edge
netedit.modifyAttribute(0, "3 7 1", True)

# rebuild network
netedit.rebuildNetwork()

# inspect crossing again after recomputing
netedit.leftClick(referencePosition, 280, 225)

# Change Edges with the same edges as another crossing (Duplicate
# crossings aren't allowed, see Ticket #4043
netedit.modifyAttribute(0, "4 8", True)

# rebuild network
netedit.rebuildNetwork()

# inspect crossing again after recomputing
netedit.leftClick(referencePosition, 280, 225)

# Change Edges to a single edge
netedit.modifyAttribute(0, "3", True)

# rebuild network
netedit.rebuildNetwork()

# Check undos
netedit.undo(referencePosition, 2)

# rebuild network
netedit.rebuildNetwork()

# Check redos
netedit.redo(referencePosition, 2)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
