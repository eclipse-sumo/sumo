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

# apply zoom (to see all POIS)
netedit.setZoom("0", "0", "80")

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect POIs
netedit.leftClick(match, 400, 400)

# Set invalid color
netedit.modifyAttribute(0, "Jren")

# Set valid color
netedit.modifyAttribute(0, "green")

# Set type
netedit.modifyAttribute(1, "common type")

# Set invalid layer
netedit.modifyAttribute(2, "dummyLayer")

# Set layer
netedit.modifyAttribute(2, "1")

# Set invalid width
netedit.modifyAttribute(3, "-3")

# Set width
netedit.modifyAttribute(3, "10")

# Set invalid height
netedit.modifyAttribute(4, "-5")

# Set height
netedit.modifyAttribute(4, "10")

# Set invalid imgfile
netedit.modifyAttribute(5, "paris.ico")

# Set imgfile
netedit.modifyAttribute(5, "berlin_icon.ico")

# Set invalid angle
netedit.modifyAttribute(6, "dummyAngle")

# Set valid angle
netedit.modifyAttribute(6, "180")

# go to select mode
netedit.selectMode()

# clear selection
netedit.selectionClear()

# Check undos and redos
netedit.undo(match, 13)
netedit.redo(match, 13)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
