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

# select first crossing
netedit.leftClick(referencePosition, netedit.positions.network.crossing.left)

# select second crossing
netedit.leftClick(referencePosition, netedit.positions.network.crossing.right)

# go to inspect mode
netedit.changeMode("inspect")

# inspect first crossing
netedit.leftClick(referencePosition, netedit.positions.network.crossing.left)

# Change shape with a non valid value
netedit.modifyAttributeOverlapped(netedit.attrs.crossing.inspectSelection.width, "dummyWidth")

# Change shape with a non valid value
netedit.modifyAttributeOverlapped(netedit.attrs.crossing.inspectSelection.width, "-5")

# Change shape with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.crossing.inspectSelection.width, "8.1")

# Check undo redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save Netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
