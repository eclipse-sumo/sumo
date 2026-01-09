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

# go to additional mode
netedit.changeMode("additional")

# select calibrator
netedit.changeElement("additionalFrame", "calibrator")

# change center view
netedit.modifyBoolAttribute(netedit.attrs.calibrator.create.center)

# create calibrator
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)

# go to inspect mode
netedit.changeMode("inspect")

# inspect calibrator
netedit.leftClickOffset(referencePosition, netedit.positions.elements.edge0, -30, 0)

# Change parameter id with a non valid value (dummy)
netedit.modifyAttributeOverlapped(netedit.attrs.calibrator.inspect.pos, "dummyPos")

# Change parameter id with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.calibrator.inspect.pos, "-12")

# Change parameter id with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.calibrator.inspect.pos, "5000")

# Change parameter id with a valid value
netedit.modifyAttributeOverlapped(netedit.attrs.calibrator.inspect.pos, "10")

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
