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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select E1
netedit.changeAdditional("e1Detector")

# create E1
netedit.leftClick(referencePosition, 250, 250)

# change to move mode
netedit.moveMode()

# move E1 to right
netedit.moveElement(referencePosition, 120, 250, 250, 250)

# go to inspect mode
netedit.inspectMode()

# inspect E1
netedit.leftClick(referencePosition, 350, 250)

# block additional
netedit.modifyBoolAttribute(14)

# change to move mode
netedit.moveMode()

# try to move E1 to right (must be blocked)
netedit.moveElement(referencePosition, 250, 250, 350, 250)

# go to inspect mode
netedit.inspectMode()

# inspect E1
netedit.leftClick(referencePosition, 350, 250)

# unblock additional
netedit.modifyBoolAttribute(14)

# change to move mode
netedit.moveMode()

# move E1 to right (must be allowed)
netedit.moveElement(referencePosition, 250, 250, 350, 250)

# Check undos and redos
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
