#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
# @date    2019-07-16

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

# go to demand mode
netedit.supermodeDemand()

# go to stop mode
netedit.stopMode()

# change stop type with a valid value
netedit.changeStopType("stopParkingArea")

# disable duration
netedit.changeDefaultBoolValue(netedit.attrs.stopParkingArea.create.durationEnable)

# create stop
netedit.leftClick(referencePosition, netedit.positions.demandElements.parkingArea.x,
                  netedit.positions.demandElements.parkingArea.y)

# enable duration
netedit.changeDefaultBoolValue(netedit.attrs.stopParkingArea.create.durationEnable)

# create stop
netedit.leftClick(referencePosition, netedit.positions.demandElements.parkingArea.x +
                  1, netedit.positions.demandElements.parkingArea.y)

# set invalid value
netedit.changeDefaultValue(netedit.attrs.stopParkingArea.create.duration, "dummyValue")

# try to create stop
netedit.leftClick(referencePosition, netedit.positions.demandElements.parkingArea.x +
                  2, netedit.positions.demandElements.parkingArea.y)

# set invalid value
netedit.changeDefaultValue(netedit.attrs.stopParkingArea.create.duration, "-20")

# try to create stop
netedit.leftClick(referencePosition, netedit.positions.demandElements.parkingArea.x +
                  3, netedit.positions.demandElements.parkingArea.y)

# set valid value
netedit.changeDefaultValue(netedit.attrs.stopParkingArea.create.duration, "0")

# try to create stop
netedit.leftClick(referencePosition, netedit.positions.demandElements.parkingArea.x +
                  4, netedit.positions.demandElements.parkingArea.y)

# set valid value
netedit.changeDefaultValue(netedit.attrs.stopParkingArea.create.duration, "22.33")

# try to create stop
netedit.leftClick(referencePosition, netedit.positions.demandElements.parkingArea.x +
                  5, netedit.positions.demandElements.parkingArea.y)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
