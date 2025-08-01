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

# select E3
netedit.changeElement("additionalFrame", "entryExitDetector")

# set invalid timeTreshold
netedit.modifyAttribute(netedit.attrs.E3.create.timeThreshold, "dummyTimeTreshold")

# try to create E3 with invalid timeTreshold
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA)

# set invalid timeTreshold
netedit.modifyAttribute(netedit.attrs.E3.create.timeThreshold, "-4")

# try to create E3 with invalid timeTreshold
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA)

# set valid timeTreshold
netedit.modifyAttribute(netedit.attrs.E3.create.timeThreshold, "5")

# create E3 with valid timeTreshold
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA)

# select entry detector
netedit.changeElement("additionalFrame", "detExit")

# Create entry detector with default value
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA)
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1)

# select exit detector
netedit.changeElement("additionalFrame", "detEntry")

netedit.leftClick(referencePosition, netedit.positions.elements.additionals.squaredA)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
