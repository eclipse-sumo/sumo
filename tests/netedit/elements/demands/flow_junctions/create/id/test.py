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

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to demand mode
netedit.changeSupermode("demand")

# go to vehicle mode
netedit.changeMode("vehicle")

# select trip over junctions
netedit.changeElement("vehicleFrame", "flow (from-to junctions)")

# try to create trip
netedit.leftClick(referencePosition, netedit.positions.elements.junction0)
netedit.leftClick(referencePosition, netedit.positions.elements.junction3)

# press enter to create trip
netedit.typeKey('enter')

# set invalid id
netedit.modifyAttribute(netedit.attrs.flowJunction.create.id, "%%;;%%%%")

# try to create trip
netedit.leftClick(referencePosition, netedit.positions.elements.junction0)
netedit.leftClick(referencePosition, netedit.positions.elements.junction3)

# press enter to create trip
netedit.typeKey('enter')

# set valid id
netedit.modifyAttribute(netedit.attrs.flowJunction.create.id, "")

# create trip
netedit.leftClick(referencePosition, netedit.positions.elements.junction0)
netedit.leftClick(referencePosition, netedit.positions.elements.junction3)

# press enter to create trip
netedit.typeKey('enter')

# set empty id
netedit.modifyAttribute(netedit.attrs.flowJunction.create.id, "f_0")

# create trip
netedit.leftClick(referencePosition, netedit.positions.elements.junction0)
netedit.leftClick(referencePosition, netedit.positions.elements.junction3)

# press enter to create trip
netedit.typeKey('enter')

# set empty id
netedit.modifyAttribute(netedit.attrs.flowJunction.create.id, "customID")

# create trip
netedit.leftClick(referencePosition, netedit.positions.elements.junction0)
netedit.leftClick(referencePosition, netedit.positions.elements.junction3)

# press enter to create trip
netedit.typeKey('enter')

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
