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

# zoom in central node
netedit.setZoom("50", "50", "50")

# Rebuild network
netedit.rebuildNetwork()

# set crossing mode
netedit.crossingMode()

# select central node
netedit.leftClick(match, 325, 225)

# select a single edge edges and create a split crossing
netedit.leftClick(match, 150, 200)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 150, 250)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 500, 200)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 500, 250)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 250, 70)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 350, 70)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 250, 400)
netedit.createCrossing()
netedit.rebuildNetwork()

# select a single edge edges and create a split crossing
netedit.leftClick(match, 350, 400)
netedit.createCrossing()
netedit.rebuildNetwork()

# Check undo redo
netedit.undo(match, 8)
netedit.redo(match, 8)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
