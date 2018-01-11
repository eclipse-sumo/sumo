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
netedit.changeShape("poly")

# change color using dialog
netedit.changeColorUsingDialog(2, 5)

# create polygon
netedit.createSquaredPoly(match, 100, 50, 100, True)

# change color manually (invalid)
netedit.modifyShapeDefaultValue(3, "Vlue")

# try to create polygon
netedit.createSquaredPoly(match, 200, 50, 100, True)

# change color manually (valid)
netedit.modifyShapeDefaultValue(3, "red")

# create polygon
netedit.createSquaredPoly(match, 200, 50, 100, True)

# change block
netedit.modifyShapeDefaultBoolValue(4)

# create polygon
netedit.createSquaredPoly(match, 300, 50, 100, True)

# change layer (invalid)
netedit.modifyShapeDefaultValue(5, "dummyLayer")

# try to create polygon
netedit.createSquaredPoly(match, 400, 50, 100, True)

# change layer (valid)
netedit.modifyShapeDefaultValue(5, "2")

# create polygon
netedit.createSquaredPoly(match, 400, 50, 100, True)

# change type
netedit.modifyShapeDefaultValue(6, "poly_test")

# create polygon
netedit.createSquaredPoly(match, 500, 50, 100, True)

# change imgfile (invalid)
netedit.modifyShapeDefaultValue(7, "%%$%$&$%$%$")

# try to create polygon
netedit.createSquaredPoly(match, 600, 50, 100, True)

# change imgfile (inexistent)
netedit.modifyShapeDefaultValue(7, "paris.ico")

# try to create polygon
netedit.createSquaredPoly(match, 600, 50, 100, True)

# change imgfile (valid)
netedit.modifyShapeDefaultValue(7, "berlin_icon.ico")

# create polygon
netedit.createSquaredPoly(match, 600, 50, 100, True)

# reset imgfile
netedit.modifyShapeDefaultValue(7, "")

# change angle (invalid)
netedit.modifyShapeDefaultValue(8, "dummyAngle")

# try to create polygon
netedit.createSquaredPoly(match, 100, 150, 100, True)

# change angle (valid, but > 360)
netedit.modifyShapeDefaultValue(8, "365")

# try to create polygon
netedit.createSquaredPoly(match, 100, 150, 100, True)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(8, "-5")

# create polygon
netedit.createSquaredPoly(match, 200, 150, 100, True)

# change angle (0 < angle < 360)
netedit.modifyShapeDefaultValue(8, "5")

# create polygon
netedit.createSquaredPoly(match, 300, 150, 100, True)

# reset angle
netedit.modifyShapeDefaultValue(8, "0")

# change block move
netedit.modifyShapeDefaultBoolValue(10)

# create polygon
netedit.createSquaredPoly(match, 400, 150, 100, True)

# change block shape
netedit.modifyShapeDefaultBoolValue(11)

# create polygon
netedit.createSquaredPoly(match, 500, 150, 100, True)

# reset block shape
netedit.modifyShapeDefaultBoolValue(11)

# create open polygon
netedit.createSquaredPoly(match, 600, 150, 100, False)

# force close polygon
netedit.modifyShapeDefaultBoolValue(12)

# create close polygon with force close enabled
netedit.createSquaredPoly(match, 100, 350, 100, True)

# create open polygon with force close enabled (will be closed)
netedit.createSquaredPoly(match, 200, 350, 100, True)

# create polygon clicking in the same points
netedit.createSquaredPoly(match, 300, 350, 0, True)

# create rectangled polygon with area 0
netedit.createRectangledPoly(match, 400, 350, 100, 0, True)

# create open line polygon
netedit.createLinePoly(match, 500, 350, 100, 100, True)

# Check undo redo
netedit.undo(match, 17)
netedit.redo(match, 17)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
