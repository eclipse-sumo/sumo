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

# go to person mode
netedit.personMode()

# change person plan
netedit.changePersonPlan("walk", False)

# create route using one edge
netedit.leftClick(referencePosition, netedit.positions.demandElements.edge0.x, netedit.positions.demandElements.edge0.y)

# press enter to create route
netedit.typeEnter()

# go to person plan mode
netedit.personPlanMode()

# select person
netedit.leftClick(referencePosition, netedit.positions.demandElements.person.x,
                  netedit.positions.demandElements.person.y)

# go to person plan mode
netedit.changePersonPlanMode("walk")

# create walkEdgeBusStop
netedit.leftClick(referencePosition, 295, 50)

# press enter to create route
netedit.typeEnter()

# go to delete mode
netedit.deleteMode()

# delete walkEdgeBusStop
netedit.leftClick(referencePosition, netedit.positions.demandElements.planBusStop.x,
                  netedit.positions.demandElements.planBusStop.y)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
