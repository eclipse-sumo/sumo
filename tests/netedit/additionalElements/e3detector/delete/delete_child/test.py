#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2020 German Aerospace Center (DLR) and others.
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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeElement("e3Detector")

# create E3
netedit.leftClick(referencePosition, 100, 50)

# select entry detector
netedit.changeElement("detEntry")

# Create Entry detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 100, 200)

# select Exit detector
netedit.changeElement("detExit")

# Create Exit detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 250, 200)

# Undo creation of E3, Entry and Exit
netedit.leftClick(referencePosition, 0, 0)
netedit.undo(referencePosition, 3)

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeElement("e3Detector")

# create E3
netedit.leftClick(referencePosition, 100, 50)

# select entry detector
netedit.changeElement("detEntry")

# Create Entry detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 100, 200)

# select Exit detector
netedit.changeElement("detExit")

# Create Exit detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 250, 200)

# Undo creation of Entry and Exit
netedit.leftClick(referencePosition, 0, 0)
netedit.undo(referencePosition, 2)

# go to additional mode
netedit.additionalMode()

# select entry detector
netedit.changeElement("detEntry")

# Create Entry detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 100, 200)

# select Exit detector
netedit.changeElement("detExit")

# Create Exit detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 250, 200)

# Change to delete
netedit.deleteMode()

# Delete the four Entry/exits
netedit.leftClick(referencePosition, 100, 200)
netedit.leftClick(referencePosition, 250, 200)
netedit.leftClick(referencePosition, 450, 200)
netedit.leftClick(referencePosition, 600, 200)

# Check undo redo
netedit.undo(referencePosition, 7)
netedit.redo(referencePosition, 7)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
