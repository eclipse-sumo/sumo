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

# select busStop
netedit.changeElement("additionalFrame", "busStop")

# create busStop in mode "Center"
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1)

# go to inspect mode
netedit.changeMode("inspect")

# inspect first busStop
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.busStop)

# Change parameter length with a non valid value (throw warning)
netedit.modifyAttributeOverlapped(netedit.attrs.busStop.inspect.parkingLength, "dummylength")

# Change parameter length with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.busStop.inspect.parkingLength, "-7")

# Change parameter length with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.busStop.inspect.parkingLength, "2.5")

# Change parameter length with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.busStop.inspect.parkingLength, "10")

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
