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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--new', '--gui-testing-debug-gl'])

# Change to create edge mode
netedit.createEdgeMode()

# Create one way edge
netedit.leftClick(referencePosition, -50, 50)
netedit.leftClick(referencePosition, 50, 50)

# try to create an edge with the same start and end (musn't be allowed)
netedit.leftClick(referencePosition, -50, 50)
netedit.leftClick(referencePosition, -50, 50)
netedit.cancelEdge()

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
