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

# rebuild network
netedit.rebuildNetwork()

# inspect central node
netedit.leftClick(referencePosition, 325, 250)

# change position with a non valid value
netedit.modifyAttribute(1, "dummy position")

# change position with a non valid value (another junction in the same position)
# BUG #
# netedit.modifyAttribute(1, "0.00,50.00")

# avoid merging
# BUG #
# netedit.typeTwoKeys("n", Key.ALT)

# change position with a valid value
netedit.modifyAttribute(1, "40.00,40.00")

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 1)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
