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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to shape mode
netedit.shapeMode()

# go to additional mode
netedit.changeShape("poly")

# create polygon
netedit.createSquaredPoly(referencePosition, 100, 150, 100, True)

# create polygon
netedit.createSquaredPoly(referencePosition, 200, 150, 100, True)

# enable block shape
netedit.modifyShapeDefaultBoolValue(12)

# create polygon blocked
netedit.createSquaredPoly(referencePosition, 300, 150, 100, True)

# disable block shape
netedit.modifyShapeDefaultBoolValue(12)

# enable block move
netedit.modifyShapeDefaultBoolValue(11)

# create polygon
netedit.createSquaredPoly(referencePosition, 400, 150, 100, True)

# enable block shape
netedit.modifyShapeDefaultBoolValue(12)

# create polygon
netedit.createSquaredPoly(referencePosition, 500, 150, 100, True)

# go to move mode
netedit.moveMode()

# move first polygon (only a existent vertex will be moved
netedit.moveElement(referencePosition, -90, 120, -90, 300)

# move second polygon (only a verte will be moved)
netedit.moveElement(referencePosition, 10, 120, 10, 300)

# move third polygon (entre shape will be moved)
netedit.moveElement(referencePosition, 130, 120, 130, 300)

# move four polygon (will not be moved
netedit.moveElement(referencePosition, 260, 120, 260, 300)

# move five polygon (will not be moved
netedit.moveElement(referencePosition, 380, 120, 380, 300)

# Check undo redo
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# save shapes
netedit.saveShapes()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
