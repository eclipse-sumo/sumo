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

# create E3 1
netedit.leftClick(referencePosition, 100, 50)

# create E3 2
netedit.leftClick(referencePosition, 200, 50)

# select entry detector
netedit.changeElement("detEntry")

# Create Entry detector for E3 3
netedit.selectAdditionalChild(6, 2)
netedit.leftClick(referencePosition, 75, 200)

# Create Entry detectors for E3 4
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(referencePosition, 250, 200)

# Change to delete
netedit.deleteMode()

# delete created E3 1
netedit.leftClick(referencePosition, 100, 50)

# delete Entry detector of E3 2 (E3 will not be removed)
netedit.leftClick(referencePosition, 250, 200)

# delete loaded E3 1
netedit.leftClick(referencePosition, 400, 50)

# delete Entry detector of loaded E3 2 (E3 will not be removed)
netedit.leftClick(referencePosition, 550, 200)

# delete lane with the last loaded entry
netedit.leftClick(referencePosition, 400, 300)

# Check undo
netedit.undo(referencePosition, 5)

# Change to delete
netedit.deleteMode()

# disble 'Automatically delete additionals'
netedit.changeAutomaticallyDeleteAdditionals(referencePosition)

# try to delete lane with the second loaded entry (doesn't allowed)
netedit.leftClick(referencePosition, 400, 300)

# wait warning
netedit.waitDeleteWarning()

# check redo
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
