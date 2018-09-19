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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--new'])

# Change to create edge mode
netedit.createEdgeMode()

# Create one way edge
netedit.leftClick(referencePosition, -50, 50)
netedit.leftClick(referencePosition, 50, 50)

# try to create an edge with the same start and end (musn't be allowed)
netedit.leftClick(referencePosition, -50, 50)
netedit.leftClick(referencePosition, -50, 50)
netedit.cancelEdge()

# Create two way edges
netedit.leftClick(referencePosition, 150, 50)
netedit.leftClick(referencePosition, 250, 50)
netedit.leftClick(referencePosition, 250, 50)
netedit.leftClick(referencePosition, 150, 50)

# select two-way mode
netedit.changeTwoWayOption()

# Create two way edges
netedit.leftClick(referencePosition, 350, 50)
netedit.leftClick(referencePosition, 450, 50)

# select chain mode
netedit.changeChainOption()

# create square
netedit.leftClick(referencePosition, -50, 150)
netedit.leftClick(referencePosition, 50, 150)
netedit.leftClick(referencePosition, 50, 250)
netedit.leftClick(referencePosition, -50, 250)
netedit.leftClick(referencePosition, -50, 150)
netedit.cancelEdge()

# try to create a duplicated edge (musn't be allowed)
netedit.leftClick(referencePosition, 350, 50)
netedit.leftClick(referencePosition, 450, 50)
netedit.cancelEdge()

# abort creation of edge
netedit.leftClick(referencePosition, 300, 250)
netedit.cancelEdge()

# create a circular road
netedit.leftClick(referencePosition, 300, 150)
netedit.leftClick(referencePosition, 400, 150)
netedit.leftClick(referencePosition, 400, 250)
netedit.leftClick(referencePosition, 400, 350)
netedit.leftClick(referencePosition, 300, 350)
netedit.leftClick(referencePosition, 200, 350)
netedit.leftClick(referencePosition, 200, 250)
netedit.leftClick(referencePosition, 200, 150)
netedit.leftClick(referencePosition, 300, 150)
netedit.cancelEdge()

# disable chain mode
netedit.changeChainOption()

# create a complex intersection
netedit.leftClick(referencePosition, 300, 250)
netedit.leftClick(referencePosition, 300, 150)

netedit.leftClick(referencePosition, 300, 250)
netedit.leftClick(referencePosition, 400, 250)

netedit.leftClick(referencePosition, 300, 250)
netedit.leftClick(referencePosition, 300, 350)

netedit.leftClick(referencePosition, 300, 250)
netedit.leftClick(referencePosition, 200, 250)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(referencePosition, 20)
netedit.redo(referencePosition, 20)

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
