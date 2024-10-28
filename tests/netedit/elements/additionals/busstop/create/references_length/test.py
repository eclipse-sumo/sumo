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

# select busStop
netedit.changeElement("busStop")

# set invalid length (dummy)
netedit.changeDefaultValue(netedit.attrs.busStop.create.length, "dummyLenght")

# try to create busstop with invalid length
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1)

# set invalid length (negative)
netedit.changeDefaultValue(netedit.attrs.busStop.create.length, "-20")

# try to create busstop with invalid length
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter0)

# Change length
netedit.changeDefaultValue(netedit.attrs.busStop.create.length, "5")

# create busStop in mode "Reference Left"
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter0)

# change reference to right
netedit.changeDefaultValue(netedit.attrs.busStop.create.references, "Reference Right")

# create busStop in mode "Reference Right"
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# change reference to center
netedit.changeDefaultValue(netedit.attrs.busStop.create.references, "Center")

# create busStop in mode "Center"
netedit.leftClick(referencePosition, netedit.positions.elements.edge3)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
