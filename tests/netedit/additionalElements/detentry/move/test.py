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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeElement("e3Detector")

# create E3
netedit.leftClick(referencePosition, 250, 50)

# select entry detector
netedit.changeElement("detEntry")

# Create Entry detector
netedit.leftClick(referencePosition, 250, 50)
netedit.leftClick(referencePosition, 100, 200)

# select Exit detector
netedit.changeElement("detExit")

# Create Exit detector
netedit.leftClick(referencePosition, 250, 50)
netedit.leftClick(referencePosition, 100, 250)

# apply zoom out
netedit.setZoom("25", "0", "70")

# change to move mode
netedit.moveMode()

# move Entry to left
netedit.moveElement(referencePosition, 110, 200, 50, 200)

# move back
netedit.moveElement(referencePosition, 50, 200, 120, 200)

# move Entry to right
netedit.moveElement(referencePosition, 120, 200, 250, 200)

# move back
netedit.moveElement(referencePosition, 250, 200, 120, 200)

# move Entry to left overpassing lane
netedit.moveElement(referencePosition, 120, 200, -150, 200)

# move back
netedit.moveElement(referencePosition, -105, 200, 120, 200)

# move Entry to right overpassing lane
netedit.moveElement(referencePosition, 120, 200, 580, 200)

# move back to another different position of initial
netedit.moveElement(referencePosition, 550, 200, 300, 200)

# Check undos and redos
netedit.undo(referencePosition, 11)
netedit.redo(referencePosition, 11)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
