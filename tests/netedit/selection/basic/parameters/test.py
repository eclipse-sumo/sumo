#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# go to select mode
netedit.changeMode("select")

# select all trainStops with lanes that contains "E5_0" and remove it
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "trainStop", "lane", "E5_0")
netedit.delete()

# select all chargingStations with id "cs_3" and remove it
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "chargingStation", "id", "cs_3")
netedit.delete()

# select all containerStops with id "cs_3" and remove it
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "containerStop", "id", "cs_3")
netedit.delete()

# select all busStops with startPos greater than 18
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "busStop", "endPos", ">18")
netedit.delete()

# select all busStops with startPos greater than 25
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "parkingArea", "startPos", "")
netedit.delete()

# select all busStops with lines that contains "lineToRemove" and remove it
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "busStop", "lines", "line1")
netedit.delete()

# select all busStops with lines that contains "linetoRemove" and remove it
netedit.selectStoppingPlaceItems("Additional elements", "Stopping places", "busStop", "lines", "line2")
netedit.delete()

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
