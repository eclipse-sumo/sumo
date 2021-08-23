#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2021 German Aerospace Center (DLR) and others.
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

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# select space
netedit.changeElement("space")

# select parent
netedit.selectAdditionalChild(8, 0)

# create space
netedit.leftClick(referencePosition, 250, 110)

# change to move mode
netedit.moveMode()

# move space
netedit.moveElement(referencePosition, 240, 100, 400, 100)

# go to inspect mode
netedit.inspectMode()

# inspect parkingArea
netedit.leftClick(referencePosition, 400, 100)

# block space
netedit.modifyBoolAttribute(9, False)

# change to move mode
netedit.moveMode()

# try to move space
netedit.moveElement(referencePosition, 400, 100, 90, 100)

# go to inspect mode
netedit.inspectMode()

# inspect parkingArea
netedit.leftClick(referencePosition, 100, 100)

# unblock space
netedit.modifyBoolAttribute(9, False)

# change to move mode
netedit.moveMode()

# move space
netedit.moveElement(referencePosition, 100, 100, 400, 200)

# Check undos and redos
netedit.undo(referencePosition, 6)
netedit.redo(referencePosition, 6)

# apply zoom
netedit.setZoom("25", "0", "74")

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
