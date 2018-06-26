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

# apply zoom
netedit.setZoom("25", "20", "25")

# go to additional mode
netedit.additionalMode()

# select BusStop
netedit.changeAdditional("busStop")

# create BusStop with default parameters
netedit.leftClick(match, 375, 250)

# select Access detector
netedit.changeAdditional("access")

# Create Access detector
netedit.selectAdditionalChild(7, 0)
netedit.leftClick(match, 200, 50)

# Try to create another Access in the same edge
netedit.selectAdditionalChild(7, 0)
netedit.leftClick(match, 200, 250)

# go to inspect mode
netedit.deleteMode()

# delete Access
netedit.leftClick(match, 208, 260)

# undo
netedit.undo(match, 1)

# go to inspect mode
netedit.deleteMode()

# delete busStop
netedit.leftClick(match, 420, 295)

# undo
netedit.undo(match, 1)

# go to inspect mode
netedit.deleteMode()

# delete both acces
netedit.leftClick(match, 208, 81)
netedit.leftClick(match, 208, 260)

# Check undo redo
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
