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

# go to additional mode
netedit.changeShape("poly")

# create polygon
netedit.createSquaredPoly(match, 100, 150, 100, True)

# create polygon
netedit.createSquaredPoly(match, 200, 150, 100, True)

# enable block shape
netedit.modifyShapeDefaultBoolValue(11)

# create polygon blocked
netedit.createSquaredPoly(match, 300, 150, 100, True)

# disable block shape
netedit.modifyShapeDefaultBoolValue(11)

# enable block move
netedit.modifyShapeDefaultBoolValue(10)

# create polygon
netedit.createSquaredPoly(match, 400, 150, 100, True)

# enable block shape
netedit.modifyShapeDefaultBoolValue(11)

# create polygon
netedit.createSquaredPoly(match, 500, 150, 100, True)

# go to move mode
netedit.moveMode()

# move first polygon (only a existent vertex will be moved
netedit.moveElement(match, -90, 120, -90, 300)

# move second polygon (only a verte will be moved)
netedit.moveElement(match, 10, 120, 10, 300)

# move third polygon (entre shape will be moved)
netedit.moveElement(match, 130, 120, 130, 300)

# move four polygon (will not be moved
netedit.moveElement(match, 260, 120, 260, 300)

# move five polygon (will not be moved
netedit.moveElement(match, 380, 120, 380, 300)

# Check undo redo
netedit.undo(match, 8)
netedit.redo(match, 8)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)

