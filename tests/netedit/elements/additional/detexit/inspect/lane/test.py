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

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeElement("entryExitDetector")

# create E3 with default parameters
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA.x, netedit.positions.elements.additionals.squaredA.y)

# select entry detector
netedit.changeElement("detExit")

# Create entry detector with default value
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA.x, netedit.positions.elements.additionals.squaredA.y)
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1.x, netedit.positions.elements.edgeCenter1.y)

# select exit detector
netedit.changeElement("detEntry")

netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA.x, netedit.positions.elements.additionals.squaredA.y)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2.x, netedit.positions.elements.edge2.y)

# go to inspect mode
netedit.inspectMode()

# inspect
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1.x, netedit.positions.elements.edgeCenter1.y)

# Change parameter lane with a non valid value (dummy Lane)
netedit.modifyAttribute(netedit.attrs.entryExit.inspect.lane, "dummyLane", True)

# Change parameter lane with a non valid value (Empty lane)
netedit.modifyAttribute(netedit.attrs.entryExit.inspect.lane, "", True)

# Change parameter lane with a valid value (other lane)
netedit.modifyAttribute(netedit.attrs.entryExit.inspect.lane, "EdgeCenter1_1", True)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
