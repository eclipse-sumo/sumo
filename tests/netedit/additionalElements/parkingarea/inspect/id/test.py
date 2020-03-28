#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2020 German Aerospace Center (DLR) and others.
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

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeElement("parkingArea")

# change reference to center
netedit.changeDefaultValue(11, "reference center")

# create parkingArea 1 in mode "reference center"
netedit.leftClick(referencePosition, 250, 165)

# create parkingArea 2 in mode "reference center"
netedit.leftClick(referencePosition, 450, 165)

# go to inspect mode
netedit.inspectMode()

# inspect first parkingArea
netedit.leftClick(referencePosition, 250, 200)

# Change parameter id with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "parkingArea_gneE2_1_1", True)

# Change parameter id with a non valid value (Invalid ID)
netedit.modifyAttribute(0, "Id with spaces", True)

# Change parameter id with a valid value
netedit.modifyAttribute(0, "correctID", True)

# Check undos and redos
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
