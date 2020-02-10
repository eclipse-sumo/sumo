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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeElement("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 180)

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 400, 180)

# select space
netedit.changeElement("space")

# create first space
netedit.selectAdditionalChild(8, 0)
netedit.leftClick(referencePosition, 250, 320)

# create second space
netedit.selectAdditionalChild(8, 0)
netedit.leftClick(referencePosition, 400, 320)

# go to inspect mode
netedit.inspectMode()

# inspect space
netedit.leftClick(referencePosition, 250, 300)

# Change parameter 7 with a non valid value (dummy)
netedit.modifyAttribute(7, "dummyParent", False)

# Change parameter 7 with a non valid value (empty)
netedit.modifyAttribute(7, "", False)

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "parkingArea_gneE2_0_1", False)

# Check undos and redos
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
