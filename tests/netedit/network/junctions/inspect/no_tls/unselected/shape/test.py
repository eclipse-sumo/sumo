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

# inspect central node
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center)

# set dummy shape
netedit.modifyAttribute(netedit.attrs.junction.inspect.shape, "dummy shape", False)

# change shape of junction
s = "50.73,53.24 53.96,51.11 52.86,47.49 51.11,45.34 47.75,44.61 43.91,47.30 44.23,50.67 47.44,52.88"
netedit.modifyAttribute(netedit.attrs.junction.inspect.shape, s, False)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 1)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
