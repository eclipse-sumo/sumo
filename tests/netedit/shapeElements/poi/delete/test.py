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
netedit.changeShape("poi")

# create POI
netedit.leftClick(referencePosition, 150, 50)

# go to delete mode
netedit.deleteMode()

# delete created poi
netedit.leftClick(referencePosition, 150, 50)

# delete first stacked loaded POI (Second stacked must stay)
netedit.leftClick(referencePosition, 200, 50)

# Check undos and redos
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save shapes
netedit.saveShapes()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
