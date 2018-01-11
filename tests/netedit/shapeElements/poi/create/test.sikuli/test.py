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

# change color using dialog
netedit.changeColorUsingDialog(2, 5)

# create poi
netedit.leftClick(match, 100, 50)

# change color manually (invalid)
netedit.modifyShapeDefaultValue(3, "Vlue")

# try to create POI
netedit.leftClick(match, 150, 50)

# change color manually (valid)
netedit.modifyShapeDefaultValue(3, "blue")

# create POI
netedit.leftClick(match, 150, 50)

# change type
netedit.modifyShapeDefaultValue(4, "poi_test")

# create POI
netedit.leftClick(match, 200, 50)

# change layer (invalid)
netedit.modifyShapeDefaultValue(5, "dummyLayer")

# try to create POI
netedit.leftClick(match, 200, 50)

# change layer (valid)
netedit.modifyShapeDefaultValue(5, "2")

# create POI
netedit.leftClick(match, 250, 50)

# change width (invalid, negative)
netedit.modifyShapeDefaultValue(6, "-2")

# try to create POI
netedit.leftClick(match, 300, 50)

# change width (valid)
netedit.modifyShapeDefaultValue(6, "7.5")

# create POI
netedit.leftClick(match, 300, 50)

# change height (invalid, negative)
netedit.modifyShapeDefaultValue(7, "-3")

# try to create POI
netedit.leftClick(match, 350, 50)

# change height (valid)
netedit.modifyShapeDefaultValue(7, "7.5")

# create POI
netedit.leftClick(match, 350, 50)

# change color to white (To see image)
netedit.modifyShapeDefaultValue(3, "white")

# change imgfile (invalid)
netedit.modifyShapeDefaultValue(8, "%%$%$&$%$%$")

# try to create POI
netedit.leftClick(match, 450, 50)

# change imgfile (non exist)
netedit.modifyShapeDefaultValue(8, "paris.ico")

# try to create POI
netedit.leftClick(match, 450, 50)

# change imgfile (valid)
netedit.modifyShapeDefaultValue(8, "berlin_icon.ico")

# create POI
netedit.leftClick(match, 450, 50)

# change angle (invalid)
netedit.modifyShapeDefaultValue(9, "dummyAngle")

# try to create POI
netedit.leftClick(match, 100, 130)

# change angle (valid, but > 360)
netedit.modifyShapeDefaultValue(9, "500")

# create POI
netedit.leftClick(match, 100, 130)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(9, "-27")

# create POI
netedit.leftClick(match, 250, 130)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(9, "45")

# create POI
netedit.leftClick(match, 400, 130)

# change block
netedit.modifyShapeDefaultBoolValue(11)

# create POI
netedit.leftClick(match, 550, 130)

# Check undo redo
netedit.undo(match, 11)
netedit.redo(match, 11)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
