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
netedit.leftClick(match, 250, 180)

# Change parameter 8 with an non valid value (dummy)
netedit.modifyAttribute(7, "DummyAllowed")

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(7, "")

# Change parameter 8 with a valid value (different separators)
netedit.modifyAttribute(7, "authority  army, passenger; taxi. tram")

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(7, "")

# Change parameter 8 with a valid value (empty)
netedit.modifyAttribute(7, "authority army vip passenger hov taxi bus coach tram bicycle")

# Change parameter 10 with an non valid value
netedit.modifyAttribute(9, "DummyDisallowed")

# Change parameter 10 with a valid value (empty)
netedit.modifyAttribute(9, "")

# Change parameter 10 with a valid value (different separators)
netedit.modifyAttribute(9, "authority  army, passenger; taxi. tram")

# Change parameter 10 with a valid value (empty)
netedit.modifyAttribute(9, "")

# Change parameter 10 with a valid value (empty)
netedit.modifyAttribute(9, "emergency authority army vip passenger hov bus coach tram rail_urban rail rail_electric motorcycle moped pedestrian custom1")

# Change parameter 11 with an non valid value (dummy)
netedit.modifyAttribute(10, "dummyShape")

# Change parameter 11 with a valid value (empty)
netedit.modifyAttribute(10, "")

# recompute
netedit.rebuildNetwork()

# inspect edge again after recomputing
netedit.leftClick(match, 250, 180)

# Change parameter 11 with a valid value
netedit.modifyAttribute(10, "13.112,16.22 34.19,16.11")

# recompute
netedit.rebuildNetwork()

# inspect edge again after recomputing
netedit.leftClick(match, 250, 200)

# Change parameter 12 with a non valid value (dummy)
netedit.modifyAttribute(11, "dummyLegth")

# Change parameter 12 with a non valid value (empty)
netedit.modifyAttribute(11, "")

# Change parameter 12 with a non valid value (negative)
netedit.modifyAttribute(11, "-12")

# Change parameter 12 with a valid value
netedit.modifyAttribute(11, "40")

# recompute
netedit.rebuildNetwork()

# inspect edge again after recomputing
netedit.leftClick(match, 250, 200)

# Change parameter 13 with a non valid value (dummy)
netedit.modifyAttribute(12, "dummySpread")

# Change parameter 13 with a non valid value (empty)
netedit.modifyAttribute(12, "")

# Change parameter 13 with a valid value
netedit.modifyAttribute(12, "center")

# Change parameter 15 with a valid value
netedit.modifyAttribute(14, "my own name")

# Change parameter 16 with a non valid value (dummy)
netedit.modifyAttribute(15, "dummyWidth")

# Change parameter 16 with a non valid value (empty)
netedit.modifyAttribute(15, "")

# Change parameter 16 with a non valid value (negative)
netedit.modifyAttribute(15, "-2")

# Change parameter 16 with a valid value (default)
netedit.modifyAttribute(15, "default")

# Change parameter 16 with a valid value (default)
netedit.modifyAttribute(15, "4")

# recompute
netedit.rebuildNetwork()

# inspect edge again after recomputing
netedit.leftClick(match, 250, 200)

# Change parameter 17 with a non valid value (dummy)
netedit.modifyAttribute(16, "dummyEndOffset")

# Change parameter 17 with a non valid value (emtpy)
netedit.modifyAttribute(16, "")

# Change parameter 17 with a non valid value (negative)
netedit.modifyAttribute(16, "-3")

# Change parameter 17 with a valid value
netedit.modifyAttribute(16, "2.5")

# Change parameter 18 with a non valid value (dummy)
netedit.modifyAttribute(17, "dummyShapeStart")

# Change parameter 18 with a non valid value (incomplete)
netedit.modifyAttribute(17, "34")

# Change parameter 18 with a valid value (empty)
netedit.modifyAttribute(17, "")

# Change parameter 18 with a valid value
netedit.modifyAttribute(17, "14,15.5")

# Change parameter 19 with a non valid value (dummy)
netedit.modifyAttribute(18, "dummyShapeEnd")

# Change parameter 19 with a non valid value (incomplete)
netedit.modifyAttribute(18, "24")

# Change parameter 19 with a duplicated value (See #3157)
netedit.modifyAttribute(18, "14,15.5")

# Change parameter 19 with a valid value (empty)
netedit.modifyAttribute(18, "")

# Change parameter 19 with a valid value
netedit.modifyAttribute(18, "34,15.5")

# Change parameter 20 with a valid value
netedit.modifyBoolAttribute(19)

# recompute
netedit.rebuildNetwork()

# Check undos
netedit.undo(match, 18)

# recompute
netedit.rebuildNetwork()

# check redos
netedit.redo(match, 18)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
