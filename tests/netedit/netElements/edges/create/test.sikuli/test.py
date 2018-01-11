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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, ['--new'])

# zoom
netedit.setZoom("50", "50", "50")

# rebuild network
netedit.rebuildNetwork()

# Change to create mode
netedit.createEdgeMode()

# Create two nodes
netedit.leftClick(match, 100, 200)
netedit.leftClick(match, 500, 200)

# Create another two nodes
netedit.leftClick(match, 100, 400)
netedit.leftClick(match, 500, 400)

# select two-way mode
netedit.changeTwoWayOption()

# create square
netedit.leftClick(match, 100, 400)
netedit.leftClick(match, 100, 200)
netedit.leftClick(match, 500, 200)
netedit.leftClick(match, 500, 400)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# rebuild network
netedit.rebuildNetwork()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
