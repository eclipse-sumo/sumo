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

# go to select mode
netedit.selectMode()

# select first POILane
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge0.x,  netedit.positions.additionalElements.centralEdge0.y)

# select second POILane
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge1.x,  netedit.positions.additionalElements.centralEdge1.y)

# go to inspect mode
netedit.inspectMode()

# inspect first POILane
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge0.x,  netedit.positions.additionalElements.centralEdge0.y)

# Change parameter file with a non valid value (invalid)
netedit.modifyAttribute(netedit.attrs.poiLane.inspectSelection.imgFile, "%$$%%%%%", True)

# Change parameter file with a non valid value (no exist)
netedit.modifyAttribute(netedit.attrs.poiLane.inspectSelection.imgFile, "paris.ico", True)

# Change parameter file with a valid value (valid)
netedit.modifyAttribute(netedit.attrs.poiLane.inspectSelection.imgFile, "berlin_icon.ico", True)

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
