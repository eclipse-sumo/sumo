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

# Change to create mode
netedit.createEdgeMode()

# Create two nodes
netedit.leftClick(referencePosition, 0, 400)
netedit.leftClick(referencePosition, 500, 400)

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeElement("busStop")

# create busStop in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# go to shape mode
netedit.shapeMode()

# go to additional mode
netedit.changeElement("poly")

# create polygon
netedit.createSquaredPoly(referencePosition, 100, 50, 100, True)

# go to demand mode
netedit.supermodeDemand()

# go to route mode
netedit.routeMode()

# create route using three edges
netedit.leftClick(referencePosition, 160, 250)
netedit.leftClick(referencePosition, 160, 225)

# press enter to create route
netedit.typeEnter()

# quit netedits saving all
netedit.quit(neteditProcess, True, True, True, True, True, True)
