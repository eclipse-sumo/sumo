#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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

# Change to create mode
netedit.createEdgeMode()

# Create two nodes
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.left)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.right)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.right)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.left)

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeElement("busStop")

# create busStop in mode "reference left"
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center, offsetY=20)

# go to demand mode
netedit.supermodeDemand()

# go to route mode
netedit.routeMode()

# create route using three edges
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center, offsetY=20)

# press enter to create route
netedit.typeEnter()

# Go to data supermode
netedit.supermodeData()

# change to edgeData
netedit.edgeData()

# create dataSet
netedit.createDataSet()

# create data interval
netedit.createDataInterval()

# create edgeData
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center, offsetY=20)

# change to edgeData
netedit.meanData()

# create mean data
netedit.createMeanData()

# quit netedit without saving
netedit.quit(neteditProcess, True, False, True, False, True, True, True, False, True, False)
