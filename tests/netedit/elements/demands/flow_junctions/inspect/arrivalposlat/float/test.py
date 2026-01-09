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

# change vehicle
netedit.changeElement("vehicleFrame", "flow (from-to junctions)")

# create trip using two junctions
netedit.leftClick(referencePosition, netedit.positions.elements.junction0)
netedit.leftClick(referencePosition, netedit.positions.elements.junction3)

# press enter to create route
netedit.typeKey("enter")

# go to inspect mode
netedit.changeMode("inspect")

# inspect vehicle
netedit.leftClick(referencePosition, netedit.positions.elements.demands.vehicleJunction)

# change arrivalSpeed with an invalid value
netedit.modifyAttribute(netedit.attrs.flowJunction.inspect.arrivalSpeed, "")

# change arrivalSpeed with an invalid value
netedit.modifyAttribute(netedit.attrs.flowJunction.inspect.arrivalSpeed, "dummySpeed")

# change departColor with a valid value
netedit.modifyAttribute(netedit.attrs.flowJunction.inspect.arrivalSpeed, "500")

# change arrivalSpeed with an invalid value
netedit.modifyAttribute(netedit.attrs.flowJunction.inspect.arrivalSpeed, "-10")

# change arrivalSpeed with a valid value
netedit.modifyAttribute(netedit.attrs.flowJunction.inspect.arrivalSpeed, "15.5")

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
