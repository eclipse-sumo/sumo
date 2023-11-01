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

# go to container mode
netedit.containerMode()

# change container plan
netedit.changeContainerPlan("tranship", False)

# create route using one edge
netedit.leftClick(referencePosition, netedit.positions.demandElements.edge0.x, netedit.positions.demandElements.edge0.y)
netedit.leftClick(referencePosition, netedit.positions.demandElements.edge2.x, netedit.positions.demandElements.edge2.y)

# press enter to create route
netedit.typeEnter()

# go to containerPlanMode mode
netedit.containerPlanMode()

# go to containerstopStop mode
netedit.changeContainerPlanMode("stop")

# create containerstopStop
netedit.leftClick(referencePosition, netedit.positions.demandElements.containerStop.x,
                  netedit.positions.demandElements.containerStop.y)

# set invalid container number
netedit.changeDefaultBoolValue(netedit.attrs.containerstopStop.create.durationEnable)

# create containerstopStop
netedit.leftClick(referencePosition, 182, 40)

# set invalid container number
netedit.changeDefaultBoolValue(netedit.attrs.containerstopStop.create.durationEnable)

# set invalid container number
netedit.changeDefaultValue(netedit.attrs.containerstopStop.create.duration, "dummy")

# create containerstopStop
netedit.leftClick(referencePosition, 184, 40)

# set invalid container number
netedit.changeDefaultValue(netedit.attrs.containerstopStop.create.duration, "-20")

# create containerstopStop
netedit.leftClick(referencePosition, 186, 40)

# set invalid container number
netedit.changeDefaultValue(netedit.attrs.containerstopStop.create.duration, "30.2")

# create containerstopStop
netedit.leftClick(referencePosition, 188, 40)

# press enter to create route
netedit.typeEnter()

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
