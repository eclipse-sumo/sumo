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

# go to poly mode and select poly
netedit.changeElement("shapeFrame", "jupedsim.obstacle")

# create first polygon
netedit.createSquaredShape(referencePosition, netedit.positions.elements.additionals.shapeA,
                           netedit.attrs.shape.size, True)
# go to inspect mode
netedit.changeMode("inspect")

# inspect first polygon
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.shapeA)

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(netedit.attrs.jpsObstacle.inspect.shape, "dummyShape")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(netedit.attrs.jpsObstacle.inspect.shape, "")

# Change parameter 1 with a valid value (single point)
netedit.modifyAttribute(netedit.attrs.jpsObstacle.inspect.shape, "12.00,8.00")

# Change parameter 1 with a valid value
netedit.modifyAttribute(netedit.attrs.jpsObstacle.inspect.shape,
                        "5.00,13.00 2.00,8.00 7.00,11.00 12.00,8.00")

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
