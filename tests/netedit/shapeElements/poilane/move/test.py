#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

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
netedit.changeShape("poiLane")

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
netedit.moveElement(referencePosition, -120, 215, 600, 215)

# move first POILane to left
netedit.moveElement(referencePosition, 540, 215, 300, 215)

# Check undo redo
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save shapes
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
