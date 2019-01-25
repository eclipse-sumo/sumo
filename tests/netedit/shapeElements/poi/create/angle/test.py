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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to shape mode
netedit.shapeMode()

# go to shape mode
netedit.changeShape("poi")

# change angle (invalid)
netedit.modifyShapeDefaultValue(10, "dummyAngle")

# try to create POI
netedit.leftClick(referencePosition, 100, 50)

# change angle (valid, but > 360)
netedit.modifyShapeDefaultValue(10, "500")

# create POI
netedit.leftClick(referencePosition, 150, 50)

# change angle (valid, < 0)
netedit.modifyShapeDefaultValue(10, "-27")

# create POI
netedit.leftClick(referencePosition, 200, 50)

# change angle (valid)
netedit.modifyShapeDefaultValue(10, "45")

# create POI
netedit.leftClick(referencePosition, 250, 50)

# Check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save shapes
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
