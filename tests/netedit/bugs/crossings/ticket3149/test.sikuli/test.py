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
netedit.setZoom("50", "50", "100")

# Rebuild network
netedit.rebuildNetwork()

# go to inspect mode
netedit.inspectMode()


# select first left edge and change their junction
netedit.leftClick(match, 180, 200)
netedit.modifyAttribute(1, "B")
netedit.rebuildNetwork()

# select second left edge and change their junction
netedit.leftClick(match, 180, 250)
netedit.modifyAttribute(2, "A")
netedit.rebuildNetwork()

# select first right edge and change their junction
netedit.leftClick(match, 450, 200)
netedit.modifyAttribute(2, "B")
netedit.rebuildNetwork()

# select second right edge and change their junction
netedit.leftClick(match, 450, 250)
netedit.modifyAttribute(1, "A")
netedit.rebuildNetwork()

# Check undo redo
netedit.undo(match, 4)
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 4)
netedit.rebuildNetwork()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
