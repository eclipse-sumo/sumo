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

# change width (invalid, dummy)
netedit.modifyShapeDefaultValue(6, "dummyWidth")

# try to create poi
netedit.leftClick(referencePosition, 100, 50)

# change width (invalid, negative)
netedit.modifyShapeDefaultValue(6, "-2")

# try to create poi
netedit.leftClick(referencePosition, 150, 50)

# change width (valid)
netedit.modifyShapeDefaultValue(6, "2.5")

# create poi
netedit.leftClick(referencePosition, 200, 50)

# Check undo redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save shapes
netedit.saveShapes()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
