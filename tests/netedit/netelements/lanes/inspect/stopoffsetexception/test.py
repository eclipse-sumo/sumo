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

# recompute
netedit.rebuildNetwork()

# toggle select lanes
netedit.changeEditMode(netedit.attrs.modes.network.selectLane)

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(referencePosition, 250, 165)

# Change parameter 4 with a valid value
netedit.modifyAttribute(netedit.attrs.lane.inspect.stopOffset, "12.5", True)

# Change parameter 1 with an non valid value (dummy)
netedit.modifyAttribute(netedit.attrs.lane.inspect.stopOffsetException, "DummyAllowed", True)

# Change parameter 1 with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.lane.inspect.stopOffsetException, "", True)

# Change parameter 1 with a valid value (different separators)
netedit.modifyAttribute(netedit.attrs.lane.inspect.stopOffsetException, "authority  army, passenger; taxi. tram", True)

# Change parameter 1 with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.lane.inspect.stopOffsetException, "", True)

# Change parameter 8 with a valid value (empty)
netedit.modifyAllowDisallowValue(netedit.attrs.lane.inspect.stopOffsetExceptionButton, True)

# Change parameter 1 with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.lane.inspect.stopOffsetException,
                        "authority army vip passenger hov taxi bus coach tram bicycle", True)

# recompute
netedit.rebuildNetwork()

# Check undos
netedit.undo(referencePosition, 4)

# recompute
netedit.rebuildNetwork()

# check redos
netedit.redo(referencePosition, 4)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
