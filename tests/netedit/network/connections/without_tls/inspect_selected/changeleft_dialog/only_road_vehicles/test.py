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

# Rebuild network
netedit.computeJunctions()

# go to select mode
netedit.changeMode("select")

# select all using invert
netedit.selection("invert")

# go to inspect mode again
netedit.changeMode("inspect")

# show connections
netedit.changeEditMode(netedit.attrs.modes.network.showConnections)

# inspect edge
netedit.leftClick(referencePosition, netedit.positions.network.connection.connectionA)

# Change parameter 8 with a valid value (empty)
netedit.modifyVClassDialogOverlapped_NoDisallowAll(netedit.attrs.connection.inspectSelection.changeLeftButton,
                                                   netedit.attrs.dialog.allowVClass.onlyRoadVehicles)

# Check undos
netedit.undo(referencePosition, 4)

# check redos
netedit.redo(referencePosition, 4)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
