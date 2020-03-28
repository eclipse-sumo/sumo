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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to demand mode
netedit.supermodeDemand()

# go to person mode
netedit.personMode()

# create person using three edges
netedit.leftClick(referencePosition, 274, 414)
netedit.leftClick(referencePosition, 570, 250)
netedit.leftClick(referencePosition, 180, 60)

# press enter to create person
netedit.typeEnter()

# go to delete mode
netedit.deleteMode()

# change zoom
netedit.setZoom("15", "20", "20")

# delete person
netedit.leftClick(referencePosition, 150, 295)

# Check undo
netedit.undo(referencePosition, 1)

# Change to network mode
netedit.supermodeNetwork()

# go to delete mode
netedit.deleteMode()

# try to delete an edge with demand elements
netedit.leftClick(referencePosition, 400, 295)

# wait warning
netedit.waitDeleteWarning()

# disable protect demand elemnts
netedit.changeProtectDemandElements(referencePosition)

# now delete edge with their person
netedit.leftClick(referencePosition, 400, 295)

# Check undo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save network
netedit.saveNetwork(referencePosition)

# save persons
netedit.saveRoutes(referencePosition)

# save additionals
netedit.saveAdditionals(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
