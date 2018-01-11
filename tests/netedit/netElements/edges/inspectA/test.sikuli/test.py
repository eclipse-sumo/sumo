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

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(match, 250, 170)

# Change parameter 0 with a non valid value (empty ID)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "gneE3")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correct_ID")

# Change parameter 1 with a non valid value (dummy Junction)
netedit.modifyAttribute(1, "dummy_Junction")

# Change parameter 1 with a non valid value (empty Junction)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a non valid value (same from Junction)
netedit.modifyAttribute(1, "gneJ2")

# Change parameter 1 with a value
netedit.modifyAttribute(1, "gneJ0")

# recompute
netedit.rebuildNetwork()

# Change parameter 2 with a non valid value (dummy Junction)
netedit.modifyAttribute(2, "dummy_Junction")

# Change parameter 2 with a non valid value (empty Junction)
netedit.modifyAttribute(2, "")

# Change parameter 2 with a non valid value (same to Junction)
netedit.modifyAttribute(2, "gneJ3")

# Change parameter 2 with a non valid value (two edges pararell)
netedit.modifyAttribute(2, "gneJ1")

# Restore parameter 1
netedit.modifyAttribute(1, "gneJ2")

# recompute
netedit.rebuildNetwork()

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "gneJ1")

# recompute
netedit.rebuildNetwork()

# Change parameter 2 with a non valid value (two edges pararell)
netedit.modifyAttribute(2, "gneJ0")

# Restore parameter 2
netedit.modifyAttribute(2, "gneJ3")

# recompute
netedit.rebuildNetwork()

# Change parameter 3 with a non valid value (empty speed)
netedit.modifyAttribute(3, "")

# Change parameter 3 with a non valid value (dummy speed)
netedit.modifyAttribute(3, "dummySpeed")

# Change parameter 3 with a non valid value (negative speed)
netedit.modifyAttribute(3, "-13")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "120.5")

# Change parameter 4 with a non valid value (empty priority)
netedit.modifyAttribute(4, "")

# Change parameter 4 with a non valid value (dummy priority)
netedit.modifyAttribute(4, "dummyPriority")

# Change parameter 4 with a non valid value (negative priority)
netedit.modifyAttribute(4, "-6")

# Change parameter 4 with a non valid value (float)
netedit.modifyAttribute(4, "6.4")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "4")

# Change parameter 5 with a non valid value (empty lanes)
netedit.modifyAttribute(5, "")

# Change parameter 5 with a non valid value (dummy lanes)
netedit.modifyAttribute(5, "dummyLanes")

# Change parameter 5 with a non valid value (negative lanes)
netedit.modifyAttribute(5, "-6")

# Change parameter 5 with a non valid value (float)
netedit.modifyAttribute(5, "-3.5")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "4")

# recompute
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 12)

# recompute
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 12)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
