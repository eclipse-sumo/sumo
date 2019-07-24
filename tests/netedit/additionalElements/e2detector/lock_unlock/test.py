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
netedit.changeElement("e2Detector")

# create E2
netedit.leftClick(referencePosition, 240, 255)

# change to move mode
netedit.moveMode()

# move E2 to right
netedit.moveElement(referencePosition, 130, 255, 220, 255)

# go to inspect mode
netedit.inspectMode()

# inspect E2
netedit.leftClick(referencePosition, 350, 255)

# block additional
netedit.modifyBoolAttribute(15, True)

# change to move mode
netedit.moveMode()

# try to move E2 to right (must be blocked)
netedit.moveElement(referencePosition, 250, 255, 350, 255)

# go to inspect mode
netedit.inspectMode()

# inspect E2
netedit.leftClick(referencePosition, 350, 255)

# unblock additional
netedit.modifyBoolAttribute(15, True)

# change to move mode
netedit.moveMode()

# move E2 to right (must be allowed)
netedit.moveElement(referencePosition, 250, 255, 350, 255)

# Check undos and redos
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
