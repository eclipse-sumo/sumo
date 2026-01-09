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

# go to shape mode
netedit.changeMode("shape")

# go to shape mode
netedit.changeElement("shapeFrame", "poi")

# create poi
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.shapeC)

# change color to white (To see icon)
netedit.modifyAttribute(netedit.attrs.poi.create.color, "white")

# Change parameter width with a valid value (To see icon)
netedit.modifyAttribute(netedit.attrs.poi.create.width, "10")

# Change parameter height with a valid value (To see icon)
netedit.modifyAttribute(netedit.attrs.poi.create.height, "10")

# change imgfile (valid)
netedit.modifyAttribute(netedit.attrs.poi.create.imgFile, "berlin_icon.ico")

# create poi
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.shapeD)

# go to move mode
netedit.changeMode("move")

# move
netedit.moveElementHorizontal(referencePosition, netedit.positions.elements.additionals.shapeC,
                              netedit.movements.radius)

# move
netedit.moveElement(referencePosition, netedit.positions.elements.additionals.shapeD,
                    netedit.movements.radius)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
