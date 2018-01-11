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

# go to additional mode
netedit.additionalMode()

# select vaporizer
netedit.changeAdditional("vaporizer")

# create vaporizer
netedit.leftClick(match, 250, 120)

# go to inspect mode
netedit.inspectMode()

# inspect first vaporizer
netedit.leftClick(match, 310, 200)

# Change parameter 0 with a non valid value (dummy edge)
netedit.modifyAttribute(0, "dummyEdge")

# Change parameter 0 with a valid value (different edge)
netedit.modifyAttribute(0, "gneE0")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummy")

# Change parameter 1 with a non valid value (negative)
netedit.modifyAttribute(1, "-10")

# Change parameter 1 with a non valid value (greather than end)
netedit.modifyAttribute(1, "50")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "5")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummy")

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-10")

# Change parameter 2 with a non valid value (minor than startTime)
netedit.modifyAttribute(2, "2")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "20")

# click over an empty area
netedit.leftClick(match, 0, 0)

# Check undos and redos
netedit.undo(match, 7)
netedit.redo(match, 7)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
