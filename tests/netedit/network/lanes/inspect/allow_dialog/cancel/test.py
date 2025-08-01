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

# go to inspect mode
netedit.changeMode("inspect")

# toggle select lanes
netedit.changeEditMode(netedit.attrs.modes.network.selectLane)

# inspect edge
netedit.leftClick(referencePosition, netedit.positions.network.edge.leftBot)

# Change parameter 8 with a valid value (empty)
netedit.modifyVClassDialog_Cancel(netedit.attrs.lane.inspect.allowButton,
                                  netedit.attrs.dialog.allowVClass.pedestrian)

# Check undos
netedit.undo(referencePosition, 4)

# check redos
netedit.redo(referencePosition, 4)

# save Netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
