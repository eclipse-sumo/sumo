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
# @date    2019-07-16

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

# go to demand mode
netedit.supermodeDemand()

# go to stop mode
netedit.stopMode()

# change stop type with a valid value
netedit.changeStopType("stopLane")

# create stop
netedit.leftClick(referencePosition, netedit.positions.elements.edge2.x, netedit.positions.elements.edge2.y)

# go to inspect mode
netedit.inspectMode()

# inspect stop
netedit.leftClick(referencePosition, netedit.positions.elements.demands.stopEdge.x,
                  netedit.positions.elements.demands.stopEdge.y)

# change value
netedit.modifyBoolAttribute(netedit.attrs.stopLane.inspect.durationEnable, True)

# change value
netedit.modifyBoolAttribute(netedit.attrs.stopLane.inspect.untilEnable, True)

# change value
netedit.modifyAttribute(netedit.attrs.stopLane.inspect.until, "dummyValue", True)

# change value
netedit.modifyAttribute(netedit.attrs.stopLane.inspect.until, "", True)

# change value
netedit.modifyAttribute(netedit.attrs.stopLane.inspect.until, "-5", True)

# change value
netedit.modifyAttribute(netedit.attrs.stopLane.inspect.until, "0", True)

# change value
netedit.modifyAttribute(netedit.attrs.stopLane.inspect.until, "3.5", True)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
