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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# Rebuild network
netedit.rebuildNetwork()

# show connections
netedit.changeEditMode('3')

# inspect connection
netedit.leftClick(referencePosition, 250, 138)

# Change shape with an invalid value
netedit.modifyAttribute(7, "dummyShape", True)

# Change shape with an valid value (empty)
netedit.modifyAttribute(7, "", True)

# Change shape with an valid value
netedit.modifyAttribute(7, "60.40,51.60 55.33,50.28 52.69,54.99 50.24,50.37 47.60,54.75 45.30,50.40 39.60,51.60", True)

# rebuild
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 3)

# rebuild
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 3)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
