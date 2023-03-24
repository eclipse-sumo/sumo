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

# go to route mode
netedit.routeMode()

# change to non consecutive mode
netedit.changeRouteMode("non consecutive edges")

# create edge
netedit.leftClick(referencePosition, 274, 392)
netedit.leftClick(referencePosition, 570, 250)
netedit.leftClick(referencePosition, 280, 55)

# press enter to create route
netedit.typeEnter()

# set invalid id
netedit.changeDefaultValue(netedit.attrs.route.create.id, "%%%;;;;")

# try to create route using three edges
netedit.leftClick(referencePosition, 274, 392)
netedit.leftClick(referencePosition, 570, 250)
netedit.leftClick(referencePosition, 280, 55)

# press enter to try to create route
netedit.typeEnter()

# set invalid ID
netedit.changeDefaultValue(netedit.attrs.route.create.id, "r_0")

# press enter to create route
netedit.typeEnter()

# set valid ID
netedit.changeDefaultValue(netedit.attrs.route.create.id, "customID")

# press enter to create route
netedit.typeEnter()

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
