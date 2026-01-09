#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# go to demand mode
netedit.changeSupermode("demand")

# go to container mode
netedit.changeMode("container")

# change container plan
netedit.changePlan("container", "tranship", False)

# create route using one edge
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)

# press enter to create route
netedit.typeKey("enter")

# go to container plan mode
netedit.changeMode("containerPlan")

# go to container plan mode
netedit.changeElement("containerPlanFrame", "transport")

# create containerTripEdgeEdge
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# press enter to create route
netedit.typeKey("enter")

# go to network mode
netedit.changeSupermode("network")

# go to delete mode
netedit.changeMode("delete")

# disable "Automatically delete demand elements"
netedit.protectElements()

# disable "Automatically delete additionals"
netedit.protectElements()

# create containerTripEdgeEdge
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# press enter to create route
netedit.typeKey("enter")

# Check undo redo
netedit.undo(referencePosition, 2)

# press enter to create route
netedit.typeKey("enter")

netedit.redo(referencePosition, 2)

# press enter to create route
netedit.typeKey("enter")

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
