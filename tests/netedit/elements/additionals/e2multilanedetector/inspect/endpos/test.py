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

# recompute
netedit.computeJunctions()

# go to additional mode
netedit.changeMode("additional")

# select E2
netedit.changeElement("additionalFrame", "multiLaneAreaDetector")

# create E2 with default parameters
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)
netedit.leftClick(referencePosition, netedit.positions.elements.edge1)
netedit.typeKey("enter")

# go to additional mode
netedit.changeMode("inspect")

# inspect E2
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.e2MultilaneDetector)

# Change parameter pos with a non valid value (dummy)
netedit.modifyAttributeOverlapped(netedit.attrs.E2Multilane.inspect.endPos, "dummyPos")

# Change parameter pos with a non valid value (negative)
netedit.modifyAttributeOverlapped(netedit.attrs.E2Multilane.inspect.endPos, "-5")

# Change parameter pos with a non valid value (> endPos)
netedit.modifyAttributeOverlapped(netedit.attrs.E2Multilane.inspect.endPos, "400")

# Change parameter pos with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.E2Multilane.inspect.endPos, "20")

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
