#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
netedit.changeEditMode(netedit.attrs.modes.network.showConnections)

# inspect connection
netedit.leftClick(referencePosition, 228, 164)

# Change parameter 8 with an non valid value (dummy)
netedit.modifyAttribute(netedit.attrs.connection.inspect.changeLeft, "DummyAllowed", True)

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.connection.inspect.changeLeft, "", True)

# Change parameter 8 with a valid value (different separators)
netedit.modifyAttribute(netedit.attrs.connection.inspect.changeLeft, "authority  army, passenger; taxi. tram", True)

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.connection.inspect.changeLeft, "", True)

# Change parameter 8 with a valid value (empty)
netedit.modifyAllowDisallowValue(netedit.attrs.connection.inspect.changeLeftButton, True)

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.connection.inspect.changeLeft,
                        "authority army vip passenger hov taxi bus coach tram bicycle", True)

# rebuild
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 4)

# rebuild
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 4)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
