#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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

# select E2
netedit.changeElement("laneAreaDetector")

# create E2
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1)

# Change to delete
netedit.deleteMode()

# delete created E2
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.e2Detector)

# Check undo
netedit.undo(referencePosition, 3)

# Check undo
netedit.undo(referencePosition, 3)

# Change to delete
netedit.deleteMode()

# try to delete lane with the second loaded busStop (doesn't allowed)
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)

# wait warning
netedit.waitDeleteWarning()

# disable 'Automatically delete additionals'
netedit.protectElements(referencePosition)

# try to delete lane with the second loaded busStop (doesn't allowed)
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)

# check redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
