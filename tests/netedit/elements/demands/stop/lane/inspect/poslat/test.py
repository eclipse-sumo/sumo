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

# go to stop mode
netedit.changeMode("stop")

# change stop type with a valid value
netedit.changeElement("stopFrame", "stopLane")

# create stop
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# go to inspect mode
netedit.changeMode("inspect")

# inspect stop
netedit.leftClick(referencePosition, netedit.positions.elements.demands.stopLane)

# change value
netedit.modifyAttributeOverlapped(netedit.attrs.stopLane.inspect.posLat, "dummy")

# change value
netedit.modifyAttributeOverlapped(netedit.attrs.stopLane.inspect.posLat, "")

# change value
netedit.modifyAttributeOverlapped(netedit.attrs.stopLane.inspect.posLat, "-30")

# change value
netedit.modifyAttributeOverlapped(netedit.attrs.stopLane.inspect.posLat, "6")

# change value
netedit.modifyAttributeOverlapped(netedit.attrs.stopLane.inspect.posLat, "2.3")

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
