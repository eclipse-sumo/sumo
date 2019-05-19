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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.setZoom("50", "50", "50")

# go to inspect mode
netedit.inspectMode()

# inspect first crossing
netedit.leftClick(referencePosition, 250, 225)

# set invalid edge
netedit.modifyAttribute(0, "dummy Edges", True)

# Change Edges adding a new edge
netedit.modifyAttribute(0, "3 7 1", True)

# rebuild network
netedit.rebuildNetwork()

# inspect crossing again after recomputing
netedit.leftClick(referencePosition, 291, 225)

# Change Edges with the same edges as another crossing (Duplicate
# crossings aren't allowed, see Ticket #4043
netedit.modifyAttribute(3, "4 8", True)

# rebuild network
netedit.rebuildNetwork()

# inspect crossing again after recomputing
netedit.leftClick(referencePosition, 291, 225)

# Change Edges to a single edge
netedit.modifyAttribute(3, "3", True)

# rebuild network
netedit.rebuildNetwork()

# Check undos
netedit.undo(referencePosition, 2)

# rebuild network
netedit.rebuildNetwork()

# Check redos
netedit.redo(referencePosition, 2)

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
