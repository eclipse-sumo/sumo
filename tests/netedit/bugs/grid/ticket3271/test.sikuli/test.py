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
netedit.setZoom("50", "50", "150")

# Change to create edge mode
netedit.createEdgeMode()

# Create an edge in
netedit.leftClick(match, -80, 50)
netedit.leftClick(match, 45, 50)

netedit.leftClick(match, -80, 50)
netedit.leftClick(match, -78, 215)

netedit.leftClick(match, 460, 50)
netedit.leftClick(match, 500, 50)

netedit.leftClick(match, 500, 50)
netedit.leftClick(match, 500, 180)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
