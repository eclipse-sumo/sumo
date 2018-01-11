#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select chargingStation
netedit.changeAdditional("chargingStation")

# change reference to center
netedit.modifyAdditionalDefaultValue(9, "reference center")

# create chargingStation in mode "reference center"
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move chargingStation to right
netedit.moveElement(match, 150, 260, 250, 260)

# go to inspect mode
netedit.inspectMode()

# inspect chargingStation
netedit.leftClick(match, 350, 260)

# block additional
netedit.modifyBoolAttribute(10)

# change to move mode
netedit.moveMode()

# try to move chargingStation to right (must be blocked)
netedit.moveElement(match, 250, 260, 350, 260)

# go to inspect mode
netedit.inspectMode()

# inspect chargingStation
netedit.leftClick(match, 350, 260)

# unblock additional
netedit.modifyBoolAttribute(10)

# change to move mode
netedit.moveMode()

# move chargingStation to right (must be allowed)
netedit.moveElement(match, 250, 260, 350, 260)

# Check undos and redos
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
