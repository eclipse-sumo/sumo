#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id: test.py 24005 2017-04-21 12:54:13Z palcraft $

python script used by sikulix for testing netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR/TS, Germany

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, False)

# go to select mode
netedit.selectMode()

# select all edges with allow = "all" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=all")
netedit.deleteSelectedItems()

# select all edges with allow = "passenger" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=passenger")
netedit.deleteSelectedItems()

# select all edges with allow = "taxi" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=taxi")
netedit.deleteSelectedItems()

# select all edges with allow = "bus" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=bus")
netedit.deleteSelectedItems()

# select all edges with allow = "coach" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=coach")
netedit.deleteSelectedItems()

# select all edges with allow = "delivery" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=delivery")
netedit.deleteSelectedItems()

# select all edges with allow = "truck" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=truck")
netedit.deleteSelectedItems()

# select all edges with allow = "trailer" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=trailer")
netedit.deleteSelectedItems()

# select all edges with allow = "emergency" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=emergency")
netedit.deleteSelectedItems()

# select all edges with allow = "motorcycle" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=motorcycle")
netedit.deleteSelectedItems()

# select all edges with allow = "moped" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=moped")
netedit.deleteSelectedItems()

# select all edges with allow = "bicycle" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=bicycle")
netedit.deleteSelectedItems()

# select all edges with allow = "pedestrian" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=pedestrian")
netedit.deleteSelectedItems()

# select all edges with allow = "tram" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=tram")
netedit.deleteSelectedItems()

# select all edges with allow = "rail_urban" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=rail_urban")
netedit.deleteSelectedItems()

# select all edges with allow = "rail" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=rail")
netedit.deleteSelectedItems()

# select all edges with allow = "rail_electric" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=rail_electric")
netedit.deleteSelectedItems()

# select all edges with allow = "ship" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=ship")
netedit.deleteSelectedItems()

# select all edges with allow = "evehicle" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=evehicle")
netedit.deleteSelectedItems()

# select all edges with allow = "private" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=private")
netedit.deleteSelectedItems()

# select all edges with allow = "army" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=army")
netedit.deleteSelectedItems()

# select all edges with allow = "authority" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=authority")
netedit.deleteSelectedItems()

# select all edges with allow = "vip" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=vip")
netedit.deleteSelectedItems()

# select all edges with allow = "hov" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=hov")
netedit.deleteSelectedItems()

# select all edges with allow = "custom1" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=custom1")
netedit.deleteSelectedItems()

# select all edges with allow = "custom2" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=custom2")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
