#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect containerstops
netedit.leftClick(referencePosition, 297, 270)

# change color using dialog
netedit.modifyColorAttribute(netedit.attrs.containerStop.inspectSelection.colorButton, 5, True)

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(netedit.attrs.containerStop.inspectSelection.color, "dummyColor", True)

# Change parameter 2 with a non valid value (invalid format)
netedit.modifyAttribute(netedit.attrs.containerStop.inspectSelection.color, "255,255,500", True)

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(netedit.attrs.containerStop.inspectSelection.color, "blue", True)

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(netedit.attrs.containerStop.inspectSelection.color, "125,60,200", True)

# Check undo redo
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
