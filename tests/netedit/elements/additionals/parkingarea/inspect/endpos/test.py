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

# go to additional mode
netedit.changeMode("additional")

# select parkingArea
netedit.changeElement("additionalFrame", "parkingArea")

# create parkingArea in mode "Center"
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1)

# go to inspect mode
netedit.changeMode("inspect")

# inspect first parkingArea
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.parkingArea)

# Change parameter endPos with a non valid value (dummy)
netedit.modifyAttributeOverlapped(netedit.attrs.parkingArea.inspect.endPos, "dummyEndPos")

# Change parameter endPos with a valid value (empty)
netedit.modifyAttributeOverlapped(netedit.attrs.parkingArea.inspect.endPos, "")

# Change parameter endPos with a valid value (out of range)
netedit.modifyAttributeOverlapped(netedit.attrs.parkingArea.inspect.endPos, "3000")

# Change parameter endPos with a non valid value (<startPos)
netedit.modifyAttributeOverlapped(netedit.attrs.parkingArea.inspect.endPos, "10")

# Change parameter endPos with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.parkingArea.inspect.endPos, "30")

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
