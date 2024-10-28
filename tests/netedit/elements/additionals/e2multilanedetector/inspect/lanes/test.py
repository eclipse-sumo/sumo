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

# recompute
netedit.rebuildNetwork()

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeElement("multiLaneAreaDetector")

# create E2 with default parameters
netedit.leftClick(referencePosition, netedit.positions.tmp)
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)
netedit.leftClick(referencePosition, netedit.positions.elements.edge1)
netedit.typeEnter()

# go to additional mode
netedit.inspectMode()

# inspect E2
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.e2MultilaneDetector)

# Change parameter lanes with a non valid value (dummy
netedit.modifyAttribute(netedit.attrs.E2Multilane.inspect.lanes, "dummyLane1 dummyLane2", True)

# Change parameter lanes with a non valid value (Invalid ID)
netedit.modifyAttribute(netedit.attrs.E2Multilane.inspect.lanes, "Id with spaces", True)

# Change parameter lanes with a non valid value (single lane)
netedit.modifyAttribute(netedit.attrs.E2Multilane.inspect.lanes, "top_0", True)

# Change parameter lanes with a non valid value (non consecutive lanes)
netedit.modifyAttribute(netedit.attrs.E2Multilane.inspect.lanes, "EdgeCenter1_1 bot_0", True)

# Change parameter lanes with a valid value
netedit.modifyAttribute(netedit.attrs.E2Multilane.inspect.lanes, "EdgeCenter0_1 EdgeCenter1_1", True)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
