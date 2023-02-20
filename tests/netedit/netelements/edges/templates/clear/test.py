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

# Change to create mode
netedit.createEdgeMode()

# set attribute
netedit.changeDefaultValue(netedit.attrs.edge.create.numLanes, "3")

# set attribute
netedit.changeDefaultValue(netedit.attrs.edge.create.priority, "15")

# set attribute
netedit.changeDefaultValue(netedit.attrs.edge.create.allow, "pedestrian bus")

# Create two nodes
netedit.leftClick(referencePosition, 87, 108)
netedit.leftClick(referencePosition, 510, 108)

# go to inspect mode
netedit.inspectMode()

# inspect
netedit.leftClick(referencePosition, 250, 120)

# create template
netedit.changeDefaultBoolValue(netedit.attrs.edge.template.create)

# Change to create mode
netedit.createEdgeMode()

# Create two nodes
netedit.leftClick(referencePosition, 87, 350)
netedit.leftClick(referencePosition, 510, 350)

# go to inspect mode
netedit.inspectMode()

# inspect
netedit.leftClick(referencePosition, 250, 120)

# clear template
netedit.changeDefaultBoolValue(netedit.attrs.edge.template.clear)

# Check undo and redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# rebuild network
netedit.rebuildNetwork()

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
