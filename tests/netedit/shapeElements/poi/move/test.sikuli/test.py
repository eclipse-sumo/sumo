#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to shape mode
netedit.shapeMode()

# go to shape mode
netedit.changeShape("poi")

# create poi
netedit.leftClick(match, 100, 100)

# change color to white (To see icon)
netedit.modifyShapeDefaultValue(3, "white")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "10")

# Change parameter 6 with a valid value
netedit.modifyAttribute(7, "10")

# change imgfile (valid)
netedit.modifyShapeDefaultValue(8, "berlin_icon.ico")

# create poi
netedit.leftClick(match, 100, 350)

# go to move mode
netedit.moveMode()

# move first POI to left down
netedit.moveElement(match, -80, 55, 200, 60)

# move second POI to left up
netedit.moveElement(match, -80, 400, 200, 300)

# Check undo redo
netedit.undo(match, 2)
netedit.redo(match, 2)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
