#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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

# select parkingArea
netedit.changeElement("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 475, 256)

# Change to delete
netedit.deleteMode()

# disable 'Automatically delete additionals'
netedit.changeProtectAdditionalElements(referencePosition)

# delete created parkingArea
netedit.leftClick(referencePosition, 521, 280)

# delete first loaded parkingArea
netedit.leftClick(referencePosition, 360, 8)

# delete lane with the second loaded parkingArea
netedit.leftClick(referencePosition, 520, 415)

# Check undo
netedit.undo(referencePosition, 3)

# Change to delete
netedit.deleteMode()

# enable 'Automatically delete additionals'
netedit.changeProtectAdditionalElements(referencePosition)

# try to delete lane with the second loaded parkingArea (doesn't allowed)
netedit.leftClick(referencePosition, 520, 415)

# wait warning
netedit.waitDeleteWarning()

# check redo
netedit.redo(referencePosition, 3)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
