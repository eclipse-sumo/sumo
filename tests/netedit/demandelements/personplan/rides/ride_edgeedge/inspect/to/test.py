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
# @date    2019-07-16

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

# go to demand mode
netedit.supermodeDemand()

# go to person mode
netedit.personMode()

# change person plan
netedit.changePersonPlan("walk: edge->edge", False)

# create route using two one
netedit.leftClick(referencePosition, 274, 400)

# press enter to create route
netedit.typeEnter()

# go to rideEdgeEdge mode
netedit.personPlanMode()

# select person
netedit.leftClick(referencePosition, 92, 412)

# go to rideEdgeEdge mode
netedit.changePersonPlanMode("ride: edge->edge")

# create rideEdgeEdge
netedit.leftClick(referencePosition, 180, 55)

# press enter to create route
netedit.typeEnter()

# go to inspect mode
netedit.inspectMode()

# inspect rideEdgeEdge
netedit.leftClick(referencePosition, 430, 65)

# change depart with an invalid value
netedit.modifyAttribute(netedit.attrs.rideEdgeEdge.inspect.to, "dummy", False)

# change depart with an invalid value
netedit.modifyAttribute(netedit.attrs.rideEdgeEdge.inspect.to, "gneE4", False)

# Check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
