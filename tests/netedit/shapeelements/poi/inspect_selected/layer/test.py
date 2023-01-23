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

# select first POI
netedit.leftClick(referencePosition, 103, 57)

# select second POI
netedit.leftClick(referencePosition, 153, 57)

# go to inspect mode
netedit.inspectMode()

# inspect first POI
netedit.leftClick(referencePosition, 103, 57)

# Change parameter 5 with a non valid value
netedit.modifyAttribute(netedit.attrs.POI.inspectSelection.layer, "dummyLayer", False)

# Change parameter 5 with a valid value (negative)
netedit.modifyAttribute(netedit.attrs.POI.inspectSelection.layer, "-2", False)

# Change parameter 5 with a valid value (default)
netedit.modifyAttribute(netedit.attrs.POI.inspectSelection.layer, "default", False)

# Change parameter 5 with a valid value
netedit.modifyAttribute(netedit.attrs.POI.inspectSelection.layer, "2", False)

# Check undos and redos
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
