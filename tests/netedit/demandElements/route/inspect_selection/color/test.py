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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to demand mode
netedit.supermodeDemand()

# go to route mode
netedit.routeMode()

# create route using three edges
netedit.leftClick(referencePosition, 274, 414)
netedit.leftClick(referencePosition, 570, 250)

# press enter to create route
netedit.typeEnter()

# create route using three edges
netedit.leftClick(referencePosition, 274, 388)

# press enter to create route
netedit.typeEnter()

# go to select mode
netedit.selectMode()

# select both routes
netedit.leftClick(referencePosition, 274, 414)
netedit.leftClick(referencePosition, 274, 388)

# go to inspect mode
netedit.inspectMode()

# inspect route
netedit.leftClick(referencePosition, 280, 417)

# Change parameter color with a non valid value (empty)
netedit.modifyAttribute(1, "", True)

# Change parameter color with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyColor", True)

# Change parameter color with a valid value
netedit.modifyAttribute(1, "12,15,30", True)

# Change parameter color with a valid value
netedit.modifyAttribute(1, "red", True)

# Check undo redo
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save routes
netedit.saveRoutes(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
