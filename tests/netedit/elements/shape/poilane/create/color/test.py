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

# go to shape mode
netedit.shapeMode()

# go to shape mode
netedit.changeElement("poiLane")

# change color using dialog
netedit.changeColorUsingDialog(netedit.attrs.poiLane.create.colorButton, 5)

# create poi
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge0.x,
                  netedit.positions.additionalElements.centralEdge0.y)

# change color manually (invalid)
netedit.changeDefaultValue(netedit.attrs.poiLane.create.color, "Vlue")

# try to create POI
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge1.x,
                  netedit.positions.additionalElements.centralEdge1.y)

# change color manually (valid)
netedit.changeDefaultValue(netedit.attrs.poiLane.create.color, "blue")

# create POI
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge2.x,
                  netedit.positions.additionalElements.centralEdge2.y)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
