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

# set delete mode
netedit.deleteMode()

# remove two left edges
netedit.leftClick(match, 150, 200)
netedit.leftClick(match, 150, 250)

# Rebuild network
netedit.rebuildNetwork()

# remove two right edges
netedit.leftClick(match, 450, 200)
netedit.leftClick(match, 450, 250)

# Rebuild network
netedit.rebuildNetwork()

# remove two up edges
netedit.leftClick(match, 300, 100)
netedit.leftClick(match, 350, 100)

# Rebuild network
netedit.rebuildNetwork()

# remove two down edges
netedit.leftClick(match, 300, 350)
netedit.leftClick(match, 350, 350)

# Rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 8)

# Rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 8)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
