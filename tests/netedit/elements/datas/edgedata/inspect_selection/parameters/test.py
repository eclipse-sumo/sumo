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

# Go to data supermode
netedit.changeSupermode("data")

# change to edgeData
netedit.changeMode("edgeData")

# create dataSet
netedit.createDataSet("newDataSet")

# create data interval
netedit.createDataInterval("0", "3600")

# create edgeData
netedit.leftClickData(referencePosition, netedit.positions.elements.edge0)

# create second edgeData
netedit.leftClickData(referencePosition, netedit.positions.elements.edge1)

# go to select mode
netedit.changeMode("select")

# select all using invert
netedit.selection("invertData")

# go to inspect mode
netedit.changeMode("inspect")

# inspect edgeData
netedit.leftClickData(referencePosition, netedit.positions.elements.edge0)

# check double parameters
netedit.checkDoubleParameters(referencePosition, netedit.attrs.edgeData.inspectSelection.parameters)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
