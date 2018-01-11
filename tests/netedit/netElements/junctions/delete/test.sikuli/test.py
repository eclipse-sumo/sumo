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


# rebuild network
netedit.rebuildNetwork()

# Change to delete mode
netedit.deleteMode()

# remove one way edge
netedit.leftClick(match, 50, 50)

# remove two way edges
netedit.leftClick(match, 260, 50)

# remove two way edges
netedit.leftClick(match, 500, 50)

# remove square
netedit.leftClick(match, 60, 160)
netedit.leftClick(match, 150, 280)

# remove circular road
netedit.leftClick(match, 450, 270)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 6)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
