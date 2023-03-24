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

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# Change to create edge mode
netedit.createEdgeMode()

# select two-way mode
netedit.changeEditMode(netedit.attrs.modes.network.twoWayMode)

# select chain mode
netedit.changeEditMode(netedit.attrs.modes.network.chainMode)

# create a circular road
netedit.leftClick(referencePosition, 300, 135)
netedit.leftClick(referencePosition, 400, 135)
netedit.leftClick(referencePosition, 400, 235)
netedit.leftClick(referencePosition, 400, 335)
netedit.leftClick(referencePosition, 300, 335)
netedit.leftClick(referencePosition, 200, 335)
netedit.leftClick(referencePosition, 200, 235)
netedit.cancelEdge()

# go to select mode
netedit.selectMode()

# select all elements using invert operation
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect set of junctions
netedit.leftClick(referencePosition, 405, 225)

# Set all Junctions as traffic lighs
netedit.modifyAttribute(netedit.attrs.junction.inspectSelection.type, "traffic_light", True)

# inspect set of edges
netedit.leftClick(referencePosition, 400, 285)

# change all speed of edges
netedit.modifyAttribute(netedit.attrs.edge.inspectSelection.speed, "20", False)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(referencePosition, 9)
netedit.redo(referencePosition, 9)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
