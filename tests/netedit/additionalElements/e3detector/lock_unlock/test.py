#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeElement("e3Detector")

# create E3
netedit.leftClick(referencePosition, 250, 250)

# select entry detector
netedit.changeElement("detEntry")

# Create Entry detector (for saving)
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(referencePosition, 500, 250)

# select entry detector
netedit.changeElement("detExit")

# Create detExit detector (for saving)
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(referencePosition, 600, 250)

# change to move mode
netedit.moveMode()

# move E3 to up
netedit.moveElement(referencePosition, 250, 250, 120, 50)

# go to inspect mode
netedit.inspectMode()

# inspect E3
netedit.leftClick(referencePosition, 120, 65)

# block additional
netedit.modifyBoolAttribute(12, False)

# change to move mode
netedit.moveMode()

# try to move E3 to right (must be blocked)
netedit.moveElement(referencePosition, 120, 50, 300, 50)

# go to inspect mode
netedit.inspectMode()

# inspect E3
netedit.leftClick(referencePosition, 300, 65)

# unblock additional
netedit.modifyBoolAttribute(12, False)

# change to move mode
netedit.moveMode()

# move E3 to right (must be allowed)
netedit.moveElement(referencePosition, 300, 50, 300, 350)

# Check undos and redos
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# apply zoom
netedit.setZoom("25", "0", "74")

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
