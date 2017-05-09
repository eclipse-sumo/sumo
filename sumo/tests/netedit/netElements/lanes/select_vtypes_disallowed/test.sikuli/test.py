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

# toogle to lane selection
netedit.selectionToogleEdges()

# select all lanes with disallow = "passenger" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=passenger")
netedit.deleteSelectedItems()

# select all lanes with disallow = "taxi" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=taxi")
netedit.deleteSelectedItems()

# select all lanes with disallow = "bus" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=bus")
netedit.deleteSelectedItems()

# select all lanes with disallow = "coach" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=coach")
netedit.deleteSelectedItems()

# select all lanes with disallow = "delivery" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=delivery")
netedit.deleteSelectedItems()

# select all lanes with disallow = "truck" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=truck")
netedit.deleteSelectedItems()

# select all lanes with disallow = "trailer" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=trailer")
netedit.deleteSelectedItems()

# select all lanes with disallow = "emergency" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=emergency")
netedit.deleteSelectedItems()

# select all lanes with disallow = "motorcycle" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=motorcycle")
netedit.deleteSelectedItems()

# select all lanes with disallow = "moped" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=moped")
netedit.deleteSelectedItems()

# select all lanes with disallow = "bicycle" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=bicycle")
netedit.deleteSelectedItems()

# select all lanes with disallow = "pedestrian" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=pedestrian")
netedit.deleteSelectedItems()

# select all lanes with disallow = "tram" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=tram")
netedit.deleteSelectedItems()

# select all lanes with disallow = "rail_urban" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=rail_urban")
netedit.deleteSelectedItems()

# select all lanes with disallow = "rail" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=rail")
netedit.deleteSelectedItems()

# select all lanes with disallow = "rail_electric" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=rail_electric")
netedit.deleteSelectedItems()

# select all lanes with disallow = "ship" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=ship")
netedit.deleteSelectedItems()

# select all lanes with disallow = "evehicle" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=evehicle")
netedit.deleteSelectedItems()

# select all lanes with disallow = "private" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=private")
netedit.deleteSelectedItems()

# select all lanes with disallow = "army" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=army")
netedit.deleteSelectedItems()

# select all lanes with disallow = "authority" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=authority")
netedit.deleteSelectedItems()

# select all lanes with disallow = "vip" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=vip")
netedit.deleteSelectedItems()

# select all lanes with disallow = "hov" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=hov")
netedit.deleteSelectedItems()

# select all lanes with disallow = "custom1" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=custom1")
netedit.deleteSelectedItems()

# select all lanes with disallow = "custom2" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=custom2")
netedit.deleteSelectedItems()

# select all lanes with disallow = "all" and remove it
netedit.selectItems("Net Element", "lane", "disdisallow", "=all")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
