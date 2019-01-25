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

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeAdditional("e2Detector")

# create E2
netedit.leftClick(referencePosition, 210, 250)

# change to move mode
netedit.moveMode()

# move E2 to left
netedit.moveElement(referencePosition, 120, 250, 30, 250)

# move back
netedit.moveElement(referencePosition, 50, 250, 120, 250)

# move E2 to right
netedit.moveElement(referencePosition, 130, 250, 250, 250)

# move back
netedit.moveElement(referencePosition, 260, 250, 120, 250)

# move E2 to left overpassing lane
netedit.moveElement(referencePosition, 130, 250, -150, 250)

# move back
netedit.moveElement(referencePosition, -80, 250, 120, 250)

# move E2 to right overpassing lane
netedit.moveElement(referencePosition, 130, 250, 480, 250)

# move back to another different position of initial
netedit.moveElement(referencePosition, 500, 250, 300, 250)

# Check undos and redos
netedit.undo(referencePosition, 10)
netedit.redo(referencePosition, 10)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
