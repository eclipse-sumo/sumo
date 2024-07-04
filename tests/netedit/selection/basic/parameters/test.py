#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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

# select all trainStops with lanes that contains "E5_0" and remove it
netedit.selectItems("Additional", "trainStop", "lane", "E5_0")
netedit.deleteSelectedItems()

# select all chargingStations with id "cs_3" and remove it
netedit.selectItems("Additional", "chargingStation", "id", "cs_3")
netedit.deleteSelectedItems()

# select all containerStops with id "cs_3" and remove it
netedit.selectItems("Additional", "containerStop", "id", "cs_3")
netedit.deleteSelectedItems()

# select all busStops with startPos greater than 18
netedit.selectItems("Additional", "busStop", "endPos", ">18")
netedit.deleteSelectedItems()

# select all busStops with startPos greater than 25
netedit.selectItems("Additional", "parkingArea", "startPos", "")
netedit.deleteSelectedItems()

# select all busStops with lines that contains "lineToRemove" and remove it
netedit.selectItems("Additional", "busStop", "lines", "line1")
netedit.deleteSelectedItems()

# select all busStops with lines that contains "linetoRemove" and remove it
netedit.selectItems("Additional", "busStop", "lines", "line2")
netedit.deleteSelectedItems()

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
