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

# go to TLS mode
netedit.changeMode("TLS")

# select junction
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center)

# create TLS
netedit.createTLS()

# join TLS
netedit.joinTSL()

# select four corners
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.up)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.down)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.left)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.right)

# join tls
netedit.typeKey("enter")

# Add a second program to the same controller and select it.
netedit.copyTLS(True)

# The existing program ID and the reserved off ID are unavailable.
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoinedMultiplePrograms.programID, "")
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoinedMultiplePrograms.programID, "off")
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoinedMultiplePrograms.programID, "0")
netedit.focusOnFrame()
netedit.typeKey("enter")

# Renaming changes the position of this program in the selector.
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoinedMultiplePrograms.programID, "!weekday")
netedit.typeKey("enter")

# The editor must still select the renamed program.
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoinedMultiplePrograms.offset, "23")
netedit.typeKey("enter")
netedit.typeKey("esc")

# Save the undo result as a separate program file.
netedit.undo(referencePosition, 2)
netedit.saveFileAs(referencePosition, "trafficLights", "tll.xml", 0)

# The network must contain the renamed program again after redo.
netedit.redo(referencePosition, 2)
netedit.saveExistentFile("neteditConfig")
netedit.quit(neteditProcess)
