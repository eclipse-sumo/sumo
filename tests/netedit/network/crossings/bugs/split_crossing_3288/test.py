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
# @date    2016-11-25

# import common functions for netedit tests
import os
import sys

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# Rebuild network
netedit.computeJunctions()

# set crossing mode
netedit.changeMode("crossing")

# select central node
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center)

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftTop)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftBot)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.rightTop)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.rightBot)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.topLeft)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.topRight)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.botLeft)
netedit.typeKey("enter")
netedit.computeJunctions()

# select a single edge edges and create a split crossing
netedit.leftClick(referencePosition, netedit.positions.network.edge.botRight)
netedit.typeKey("enter")
netedit.computeJunctions()

# Check undo redo
netedit.undo(referencePosition, 8)

netedit.computeJunctions()

netedit.redo(referencePosition, 8)

# save network
netedit.saveExistentFile("network")

# press space to fix crossings
netedit.typeKey("space")

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
