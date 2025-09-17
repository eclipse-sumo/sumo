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

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# go to select mode
netedit.changeMode("select")

# select junctions
netedit.leftClick(referencePosition, netedit.positions.bugs.ticket5552.junction1)
netedit.leftClick(referencePosition, netedit.positions.bugs.ticket5552.junction2)
netedit.leftClick(referencePosition, netedit.positions.bugs.ticket5552.junction3)
netedit.leftClick(referencePosition, netedit.positions.bugs.ticket5552.junction4)
netedit.leftClick(referencePosition, netedit.positions.bugs.ticket5552.junction5)
netedit.leftClick(referencePosition, netedit.positions.bugs.ticket5552.junction6)

# join junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.computeJunctions()

# transform to roundAbout
netedit.contextualMenuOperation(referencePosition, netedit.positions.network.junction.cross.center,
                                netedit.contextualMenu.junction.splitJunctionAndReconnect)

# rebuild network
netedit.computeJunctions()

# Check undo
netedit.undo(referencePosition, 2)

# rebuild network
netedit.computeJunctions()

# Check redo
netedit.redo(referencePosition, 2)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
