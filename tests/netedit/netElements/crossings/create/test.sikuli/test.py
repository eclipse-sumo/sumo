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

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.setZoom("50", "50", "50")

# set crossing mode
netedit.crossingMode()

# select central node
netedit.leftClick(match, 325, 225)

# select two left edges and create crossing in edges 3 and 7
netedit.leftClick(match, 150, 200)
netedit.leftClick(match, 150, 250)
netedit.createCrossing()
netedit.rebuildNetwork()

# create manual crossing
netedit.modifyCrossingDefaultValue(3, "1 5")
netedit.createCrossing()
netedit.rebuildNetwork()

# try to create again the same crossing (cannot be possible, show warning
# instead)
netedit.modifyCrossingDefaultValue(3, "1 5")
netedit.createCrossing()
netedit.rebuildNetwork()

# create single crossing (fail)
netedit.modifyCrossingDefaultValue(3, "4")
netedit.createCrossing()
netedit.rebuildNetwork()

# create split crossing
netedit.modifyCrossingDefaultValue(3, "4")
netedit.createCrossing()
netedit.modifyCrossingDefaultValue(3, "8")
netedit.createCrossing()
netedit.rebuildNetwork()

# create manual crossing with different priority and width
netedit.modifyCrossingDefaultValue(3, "6 2")
netedit.modifyCrossingDefaultBoolValue(4)
netedit.modifyCrossingDefaultValue(5, "5")
netedit.createCrossing()
netedit.rebuildNetwork()

# Check buttons
netedit.leftClick(match, 150, 200)
netedit.leftClick(match, 150, 250)
netedit.crossingInvertEdges()
netedit.crossingClearEdges()

# Check undo redo
netedit.undo(match, 5)
netedit.redo(match, 5)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
