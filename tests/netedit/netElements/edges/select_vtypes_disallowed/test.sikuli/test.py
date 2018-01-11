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

# go to select mode
netedit.selectMode()

# select all edges with disallow = "all" and remove it
# (we cannot use "=all" since "all" is expanded to include all vClassess)
netedit.selectItems("Net Element", "edge", "disallow", "all")
netedit.deleteSelectedItems()

# select all edges with disallow = "passenger" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=passenger")
netedit.deleteSelectedItems()

# select all edges with disallow = "taxi" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=taxi")
netedit.deleteSelectedItems()

# select all edges with disallow = "bus" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=bus")
netedit.deleteSelectedItems()

# select all edges with disallow = "coach" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=coach")
netedit.deleteSelectedItems()

# select all edges with disallow = "delivery" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=delivery")
netedit.deleteSelectedItems()

# select all edges with disallow = "truck" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=truck")
netedit.deleteSelectedItems()

# select all edges with disallow = "trailer" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=trailer")
netedit.deleteSelectedItems()

# select all edges with disallow = "emergency" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=emergency")
netedit.deleteSelectedItems()

# select all edges with disallow = "motorcycle" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=motorcycle")
netedit.deleteSelectedItems()

# select all edges with disallow = "moped" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=moped")
netedit.deleteSelectedItems()

# select all edges with disallow = "bicycle" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=bicycle")
netedit.deleteSelectedItems()

# select all edges with disallow = "pedestrian" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=pedestrian")
netedit.deleteSelectedItems()

# select all edges with disallow = "tram" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=tram")
netedit.deleteSelectedItems()

# select all edges with disallow = "rail_urban" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=rail_urban")
netedit.deleteSelectedItems()

# select all edges with disallow = "rail" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=rail")
netedit.deleteSelectedItems()

# select all edges with disallow = "rail_electric" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=rail_electric")
netedit.deleteSelectedItems()

# select all edges with disallow = "ship" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=ship")
netedit.deleteSelectedItems()

# select all edges with disallow = "evehicle" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=evehicle")
netedit.deleteSelectedItems()

# select all edges with disallow = "private" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=private")
netedit.deleteSelectedItems()

# select all edges with disallow = "army" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=army")
netedit.deleteSelectedItems()

# select all edges with disallow = "authority" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=authority")
netedit.deleteSelectedItems()

# select all edges with disallow = "vip" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=vip")
netedit.deleteSelectedItems()

# select all edges with disallow = "hov" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=hov")
netedit.deleteSelectedItems()

# select all edges with disallow = "custom1" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=custom1")
netedit.deleteSelectedItems()

# select all edges with disallow = "custom2" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=custom2")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
