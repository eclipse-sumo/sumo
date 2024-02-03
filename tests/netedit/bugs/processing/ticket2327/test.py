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

# rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# select node 1
netedit.leftClick(referencePosition, 216, 58)

# select node 2
netedit.leftClick(referencePosition, 333, 58)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# select node 3
netedit.leftClick(referencePosition, 448, 64)

# select node 4
netedit.leftClick(referencePosition, 566, 64)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# select node 5
netedit.leftClick(referencePosition, 214, 180)

# select node 6
netedit.leftClick(referencePosition, 335, 294)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# select node 8
netedit.leftClick(referencePosition, 510, 174)

# select node 9
netedit.leftClick(referencePosition, 623, 174)

# select node 10
netedit.leftClick(referencePosition, 739, 174)

# inspect node 11
netedit.leftClick(referencePosition, 510, 294)

# inspect node 12
netedit.leftClick(referencePosition, 623, 294)

# inspect node 13
netedit.leftClick(referencePosition, 739, 294)

# inspect node 14
netedit.leftClick(referencePosition, 510, 410)

# inspect node 15
netedit.leftClick(referencePosition, 623, 410)

# inspect node 16
netedit.leftClick(referencePosition, 739, 410)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# Undo joining
netedit.undo(referencePosition, 4)

# rebuild network
netedit.rebuildNetwork()

# redo joining
netedit.redo(referencePosition, 4)

# rebuild network
netedit.rebuildNetwork()

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
