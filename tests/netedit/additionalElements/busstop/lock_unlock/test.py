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

# select busStop
netedit.changeElement("busStop")

# change reference to center
netedit.changeDefaultValue(8, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(referencePosition, 250, 255)

# change to move mode
netedit.moveMode()

# move busStop to right
netedit.moveElement(referencePosition, 150, 275, 250, 275)

# go to inspect mode
netedit.inspectMode()

# inspect busStop
netedit.leftClick(referencePosition, 350, 275)

# block additional
netedit.modifyBoolAttribute(7, True)

# change to move mode
netedit.moveMode()

# try to move busStop to right (must be blocked)
netedit.moveElement(referencePosition, 250, 275, 350, 275)

# go to inspect mode
netedit.inspectMode()

# inspect busStop
netedit.leftClick(referencePosition, 350, 275)

# unblock additional
netedit.modifyBoolAttribute(7, True)

# change to move mode
netedit.moveMode()

# move busStop to right (must be allowed)
netedit.moveElement(referencePosition, 250, 275, 350, 275)

# Check undos and redos
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
