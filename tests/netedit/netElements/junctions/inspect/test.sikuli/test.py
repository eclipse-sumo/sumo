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

# rebuild network
netedit.rebuildNetwork()

# inspect first node
netedit.leftClick(match, 50, 50)

# change ID (Duplicated)
netedit.modifyAttribute(0, "gneJ1")

# change ID empty)
netedit.modifyAttribute(0, "")

# change ID
netedit.modifyAttribute(0, "OwnID")

# change position
netedit.modifyAttribute(1, "20.00,50.00")

# change type of junction (should not be possible due is a dead_end)
netedit.modifyAttribute(2, "allway_stop")

# inspect second node
netedit.leftClick(match, 150, 50)

# change type of junction (should not be possible due is a dead_end)
netedit.modifyAttribute(2, "allway_stop")

# inspect third node
netedit.leftClick(match, 265, 50)

# change type of junction
netedit.modifyAttribute(2, "allway_stop")

# inspect four node
netedit.leftClick(match, 380, 50)

# change type of junction
netedit.modifyAttribute(2, "traffic_light_unregulated")

# inspect five node
netedit.leftClick(match, 495, 50)

# change type of junction
netedit.modifyAttribute(2, "traffic_light_right_on_red")

# inspect five node
netedit.leftClick(match, 610, 50)

# change type of junction
netedit.modifyAttribute(2, "rail_signal")

# inspect six node
netedit.leftClick(match, 50, 175)

# change type of junction
netedit.modifyAttribute(2, "rail_crossing")

# inspect seven node
netedit.leftClick(match, 150, 175)

# change type of junction
netedit.modifyAttribute(2, "priority")

# inspect six node
netedit.leftClick(match, 50, 275)

# change type of junction
netedit.modifyAttribute(2, "priority_stop")

# inspect seven node
netedit.leftClick(match, 150, 275)

# change type of junction
netedit.modifyAttribute(2, "right_before_left")

# inspect eight node
netedit.leftClick(match, 340, 170)

# change type of junction
netedit.modifyAttribute(2, "allway_stop")

# inspect nine node
netedit.leftClick(match, 450, 170)

# change type of junction
netedit.modifyAttribute(2, "zipper")

# inspect ten node
netedit.leftClick(match, 550, 170)

# change type of junction
netedit.modifyAttribute(2, "district")

# inspect eleven node
netedit.leftClick(match, 350, 275)

# change type of junction
netedit.modifyAttribute(2, "unregulated")

# inspect twelve node
netedit.leftClick(match, 450, 275)

# change type of junction
netedit.modifyAttribute(2, "internal")

# inspect thirteen node
netedit.leftClick(match, 550, 275)

# change type of junction
netedit.modifyAttribute(2, "dead_end")

# rebuild network
netedit.rebuildNetwork()

# inspect eight node
netedit.leftClick(match, 340, 180)

# change shape of junction
netedit.modifyAttribute(4, "52.85,50.00 50.54,43.93 50.50,43.16 46.01,38.46 45.53,38.12 39.59,35.49")

# inspect eleven node
netedit.leftClick(match, 350, 275)

# change radio
netedit.modifyAttribute(5, "2")

# change keep clear
netedit.modifyBoolAttribute(6)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 22)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 22)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
