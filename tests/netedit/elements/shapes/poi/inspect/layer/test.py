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

# go to shape mode
netedit.changeMode("shape")

# select POI in list of shapes
netedit.changeElement("shapeFrame", "poi")

# create first POI
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.shapeA)

# go to inspect mode
netedit.changeMode("inspect")

# inspect first POI
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.shapeA)

# Change parameter 6 with a non valid value
netedit.modifyAttribute(netedit.attrs.poi.inspect.layer, "dummyLayer")

# Change parameter 6 with a valid value (negative)
netedit.modifyAttribute(netedit.attrs.poi.inspect.layer, "-2")

# Change parameter 6 with a valid value (default)
netedit.modifyAttribute(netedit.attrs.poi.inspect.layer, "default")

# Change parameter 6 with a valid value
netedit.modifyAttribute(netedit.attrs.poi.inspect.layer, "2")

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
