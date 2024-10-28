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

# go to additional mode
netedit.additionalMode()

# select calibratorLane
netedit.changeElement("calibratorLane")

# disable center view
netedit.changeDefaultBoolValue(netedit.attrs.calibrator.create.center)

# change routeprobe with a invalid routeProbe ID
netedit.changeDefaultValue(netedit.attrs.calibrator.create.routeProbe, ";;;;;%%;;;;")

# create calibratorLane with a different routeProbe in other lane
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)

# change frequency with a different routeProbe (Valid, empty)
netedit.changeDefaultValue(netedit.attrs.calibrator.create.routeProbe, "")

# create calibratorLane with a valid parameter in other lane
netedit.leftClick(referencePosition, netedit.positions.elements.edge1)

# change routeprobe with a different routeProbe
netedit.changeDefaultValue(netedit.attrs.calibrator.create.routeProbe, "routeProbe_0")

# create calibratorLane with a valid parameter in other lane
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
