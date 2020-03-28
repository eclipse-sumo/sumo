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

# set invalid file
netedit.changeDefaultValue(5, "%%%$$%%$%")

# try to create E3 with invalid file
netedit.leftClick(referencePosition, 100, 100)

# set valid file
netedit.changeDefaultValue(5, "myOwnFile.txt")

# create E3 with valid file
netedit.leftClick(referencePosition, 100, 100)

# select entry detector
netedit.changeElement("detEntry")

# Create Entry and exit detectors for all E3 detectors (except for the first, that only have one Entry)
netedit.leftClick(referencePosition, 100, 100)  # select E3
netedit.leftClick(referencePosition, 300, 250)

# select entry detector
netedit.changeElement("detExit")

# Create Entry and exit detectors for all E3 detectors (except for the first, that only have one Entry)
netedit.leftClick(referencePosition, 100, 100)  # select E3
netedit.leftClick(referencePosition, 300, 400)

# Check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
