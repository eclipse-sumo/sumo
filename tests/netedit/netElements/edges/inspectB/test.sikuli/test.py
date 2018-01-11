#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# recompute
netedit.rebuildNetwork()

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(match, 250, 170)

# Change parameter 8 with an non valid value (dummy)
netedit.modifyAttribute(8, "DummyAllowed")

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(8, "")

# Change parameter 8 with a valid value (different separators)
netedit.modifyAttribute(8, "authority  army, passenger; taxi. tram")

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(8, "")

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(8, "authority army vip passenger hov taxi bus coach tram bicycle")

# Change parameter 10 with an non valid value
netedit.modifyAttribute(10, "DummyDisallowed")

# Change parameter 10 with a valid value (empty)
netedit.modifyAttribute(10, "")

# Change parameter 10 with a valid value (different separators)
netedit.modifyAttribute(10, "authority  army, passenger; taxi. tram")

# Change parameter 10 with a valid value (empty)
netedit.modifyAttribute(10, "")

# Change parameter 10 with a valid value (empty)
netedit.modifyAttribute(
    10, "emergency authority army vip passenger hov bus coach tram rail_urban rail rail_electric motorcycle moped pedestrian custom1")

# Change parameter 11 with an non valid value (dummy)
netedit.modifyAttribute(11, "dummyShape")

# Change parameter 11 with a valid value (empty)
netedit.modifyAttribute(11, "")

# recompute
netedit.rebuildNetwork()

# Change parameter 11 with a valid value
netedit.modifyAttribute(11, "13.112,16.22 34.19,16.11")

# recompute
netedit.rebuildNetwork()

# Change parameter 12 with a non valid value (dummy)
netedit.modifyAttribute(12, "dummyLegth")

# Change parameter 12 with a non valid value (empty)
netedit.modifyAttribute(12, "")

# Change parameter 12 with a non valid value (negative)
netedit.modifyAttribute(12, "-12")

# Change parameter 12 with a valid value
netedit.modifyAttribute(12, "40")

# recompute
netedit.rebuildNetwork()

# Change parameter 13 with a non valid value (dummy)
netedit.modifyAttribute(13, "dummySpread")

# Change parameter 13 with a non valid value (empty)
netedit.modifyAttribute(13, "")

# Change parameter 13 with a valid value
netedit.modifyAttribute(13, "center")

# Change parameter 15 with a valid value
netedit.modifyAttribute(15, "my own name")

# Change parameter 16 with a non valid value (dummy)
netedit.modifyAttribute(16, "dummyWidth")

# Change parameter 16 with a non valid value (empty)
netedit.modifyAttribute(16, "")

# Change parameter 16 with a non valid value (negative)
netedit.modifyAttribute(16, "-2")

# Change parameter 16 with a valid value (default)
netedit.modifyAttribute(16, "default")

# Change parameter 16 with a valid value (default)
netedit.modifyAttribute(16, "4")

# recompute
netedit.rebuildNetwork()

# Change parameter 17 with a non valid value (dummy)
netedit.modifyAttribute(17, "dummyEndOffset")

# Change parameter 17 with a non valid value (emtpy)
netedit.modifyAttribute(17, "")

# Change parameter 17 with a non valid value (negative)
netedit.modifyAttribute(17, "-3")

# Change parameter 17 with a valid value
netedit.modifyAttribute(17, "2.5")

# Change parameter 18 with a non valid value (dummy)
netedit.modifyAttribute(18, "dummyShapeStart")

# Change parameter 18 with a non valid value (incomplete)
netedit.modifyAttribute(18, "34")

# Change parameter 18 with a valid value (empty)
netedit.modifyAttribute(18, "")

# Change parameter 18 with a valid value
netedit.modifyAttribute(18, "14,15.5")

# Change parameter 19 with a non valid value (dummy)
netedit.modifyAttribute(19, "dummyShapeEnd")

# Change parameter 19 with a non valid value (incomplete)
netedit.modifyAttribute(19, "24")

# Change parameter 19 with a duplicated value (See #3157)
netedit.modifyAttribute(19, "14,15.5")

# Change parameter 19 with a valid value (empty)
netedit.modifyAttribute(19, "")

# Change parameter 19 with a valid value
netedit.modifyAttribute(19, "34,15.5")

# recompute
netedit.rebuildNetwork()

# Check undos
netedit.undo(match, 17)

# recompute
netedit.rebuildNetwork()

# check redos
netedit.redo(match, 17)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
