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

# select all edges with allow = "all" and remove it
# (we cannot use "=all" since "all" is expanded to include all vClassess)
netedit.selectItems("Net Element", "edge", "allow", "all")
netedit.deleteSelectedItems()

# select all edges with allow = "passenger" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=passenger")
netedit.deleteSelectedItems()

# select all edges with allow = "taxi" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=taxi")
netedit.deleteSelectedItems()

# select all edges with allow = "bus" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=bus")
netedit.deleteSelectedItems()

# select all edges with allow = "coach" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=coach")
netedit.deleteSelectedItems()

# select all edges with allow = "delivery" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=delivery")
netedit.deleteSelectedItems()

# select all edges with allow = "truck" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=truck")
netedit.deleteSelectedItems()

# select all edges with allow = "trailer" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=trailer")
netedit.deleteSelectedItems()

# select all edges with allow = "emergency" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=emergency")
netedit.deleteSelectedItems()

# select all edges with allow = "motorcycle" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=motorcycle")
netedit.deleteSelectedItems()

# select all edges with allow = "moped" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=moped")
netedit.deleteSelectedItems()

# select all edges with allow = "bicycle" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=bicycle")
netedit.deleteSelectedItems()

# select all edges with allow = "pedestrian" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=pedestrian")
netedit.deleteSelectedItems()

# select all edges with allow = "tram" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=tram")
netedit.deleteSelectedItems()

# select all edges with allow = "rail_urban" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=rail_urban")
netedit.deleteSelectedItems()

# select all edges with allow = "rail" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=rail")
netedit.deleteSelectedItems()

# select all edges with allow = "rail_electric" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=rail_electric")
netedit.deleteSelectedItems()

# select all edges with allow = "ship" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=ship")
netedit.deleteSelectedItems()

# select all edges with allow = "evehicle" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=evehicle")
netedit.deleteSelectedItems()

# select all edges with allow = "private" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=private")
netedit.deleteSelectedItems()

# select all edges with allow = "army" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=army")
netedit.deleteSelectedItems()

# select all edges with allow = "authority" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=authority")
netedit.deleteSelectedItems()

# select all edges with allow = "vip" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=vip")
netedit.deleteSelectedItems()

# select all edges with allow = "hov" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=hov")
netedit.deleteSelectedItems()

# select all edges with allow = "custom1" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=custom1")
netedit.deleteSelectedItems()

# select all edges with allow = "custom2" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=custom2")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
