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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# rebuild network
netedit.rebuildNetwork()

# inspect central node
netedit.leftClick(referencePosition, 325, 250)

# set dummy shape
netedit.modifyAttribute(4, "dummy shape", False)

# change shape of junction
netedit.modifyAttribute(4, "43.60,60.40 56.40,60.40 52.00,53.00 60.40,56.40 60.40,43.60 52.00,47.00" +
                           "56.40,39.60 43.60,39.60 48.00,47.00 39.60,43.60 39.60,56.40 48.00,53.00 43.60,60.40", False)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 1)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
