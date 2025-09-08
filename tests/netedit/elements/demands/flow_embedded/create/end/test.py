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

# go to vehicle mode
netedit.changeMode("vehicle")

# select flow
netedit.changeElement("vehicleFrame", "flow (embedded route)")

# set invalid arrival pos
netedit.modifyAttribute(netedit.attrs.flowEmbedded.create.terminate, "dummyTerminate")

# try to create flow
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# press enter to create flow
netedit.typeKey("enter")

# set invalid arrival pos
netedit.modifyAttribute(netedit.attrs.flowEmbedded.create.terminate, "end")

# press enter to create flow
netedit.typeKey("enter")

# set valid arrival pos
netedit.modifyAttribute(netedit.attrs.flowEmbedded.create.terminateOption, "dummy")

# create flow
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# press enter to create flow
netedit.typeKey("enter")

# set valid arrival pos
netedit.modifyAttribute(netedit.attrs.flowEmbedded.create.terminateOption, "-30")

# press enter to create flow
netedit.typeKey("enter")

# set valid arrival pos
netedit.modifyAttribute(netedit.attrs.flowEmbedded.create.terminateOption, "20.5")

# press enter to create flow
netedit.typeKey("enter")

# set valid arrival pos
netedit.modifyAttribute(netedit.attrs.flowEmbedded.create.terminateOption, "22")

# create flow
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# press enter to create flow
netedit.typeKey("enter")

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
