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

# select first TAZ
netedit.leftClick(referencePosition, 115, 70)

# select second TAZ
netedit.leftClick(referencePosition, 300, 70)

# go to inspect mode
netedit.inspectMode()

# inspect first TAZ
netedit.leftClick(referencePosition, 115, 70)

# change color using dialog
netedit.modifyColorAttribute(netedit.attrs.TAZ.inspectSelection.colorButton, 5, False)

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(netedit.attrs.TAZ.inspectSelection.color, "dummyColor", False)

# Change parameter 2 with a non valid value (invalid format)
netedit.modifyAttribute(netedit.attrs.TAZ.inspectSelection.color, "255,255,500", False)

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(netedit.attrs.TAZ.inspectSelection.color, "blue", False)

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(netedit.attrs.TAZ.inspectSelection.color, "125,60,200", False)

# Check undos and redos
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save TAZs
# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
