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

# select vaporizer
netedit.changeElement("additionalFrame", "vaporizer")

# disable center view
netedit.modifyBoolAttribute(netedit.attrs.vaporizer.create.center)

# set invalid end
netedit.modifyAttribute(netedit.attrs.vaporizer.create.end, "-20")

# try to create create vaporizer
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)

# set valid end
netedit.modifyAttribute(netedit.attrs.vaporizer.create.end, "20")

# create vaporizer
netedit.leftClick(referencePosition, netedit.positions.elements.edge1)

# change default start (Invalid, end > startTime)
netedit.modifyAttribute(netedit.attrs.vaporizer.create.end, "50")

# try to create invalid vaporizer (show warning)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# change default end (valid))
netedit.modifyAttribute(netedit.attrs.vaporizer.create.end, "100")

# try to create invalid vaporizer (show warning)
netedit.leftClick(referencePosition, netedit.positions.elements.edge3)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
