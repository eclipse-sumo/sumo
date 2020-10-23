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

# go to additional mode
netedit.additionalMode()

# select containerStop
netedit.changeElement("containerStop")

# change reference to center
netedit.changeDefaultValue(7, "reference center")

# create containerStop in mode "reference center"
netedit.leftClick(referencePosition, 250, 255)

# change to move mode
netedit.moveMode()

# move containerStop to left
netedit.moveElement(referencePosition, 150, 275, 50, 275)

# move back
netedit.moveElement(referencePosition, 50, 275, 150, 275)

# move containerStop to right
netedit.moveElement(referencePosition, 150, 275, 250, 275)

# move back
netedit.moveElement(referencePosition, 250, 275, 150, 275)

# move containerStop to left overpassing lane
netedit.moveElement(referencePosition, 150, 275, -100, 275)

# move back
netedit.moveElement(referencePosition, -90, 275, 150, 275)

# move containerStop to right overpassing lane
netedit.moveElement(referencePosition, 150, 275, 550, 275)

# move back to another different position of initial
netedit.moveElement(referencePosition, 500, 275, 300, 275)

# Check undos and redos
netedit.undo(referencePosition, 10)
netedit.redo(referencePosition, 10)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
