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

# go to shape mode
netedit.shapeMode()

# go to shape mode
netedit.changeElement("poiLane")

# create poiLane
netedit.leftClick(referencePosition, 150, 215)

# go to move mode
netedit.moveMode()

# move first POILane to right
netedit.moveElement(referencePosition, -20, 215, 200, 215)

# move first POILane to left
netedit.moveElement(referencePosition, 200, 215, 0, 215)

# move first POILane to left
netedit.moveElement(referencePosition, 0, 215, -180, 215)

# move first POILane to left
netedit.moveElement(referencePosition, -100, 215, 600, 215)

# move first POILane to left
netedit.moveElement(referencePosition, 530, 215, 300, 215)

# Check undo redo
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save shapes
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
