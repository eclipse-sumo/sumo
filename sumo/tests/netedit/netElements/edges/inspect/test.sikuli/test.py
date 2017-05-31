#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(match, 250, 170)

# Change parameter 0 with a non valid value (empty ID)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "gneE3")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correct_ID")

# Change parameter 1 with a non valid value (dummy Junction)
netedit.modifyAttribute(1, "dummy_Junction")

# Change parameter 1 with a non valid value (empty Junction)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a non valid value (same from Junction)
netedit.modifyAttribute(1, "gneJ2")

# Change parameter 1 with a value
netedit.modifyAttribute(1, "gneJ0")

# recompute
netedit.rebuildNetwork()

# Change parameter 2 with a non valid value (dummy Junction)
netedit.modifyAttribute(2, "dummy_Junction")

# Change parameter 2 with a non valid value (empty Junction)
netedit.modifyAttribute(2, "")

# Change parameter 2 with a non valid value (same to Junction)
netedit.modifyAttribute(2, "gneJ3")

# Change parameter 2 with a non valid value (two edges pararell)
netedit.modifyAttribute(2, "gneJ1")

# Restore parameter 1
netedit.modifyAttribute(1, "gneJ2")

# recompute
netedit.rebuildNetwork()

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "gneJ1")

# recompute
netedit.rebuildNetwork()

# Change parameter 2 with a non valid value (two edges pararell)
netedit.modifyAttribute(2, "gneJ0")

# Restore parameter 2
netedit.modifyAttribute(2, "gneJ3")

# recompute
netedit.rebuildNetwork()

# Change parameter 3 with a non valid value (empty speed)
netedit.modifyAttribute(3, "")

# Change parameter 3 with a non valid value (dummy speed)
netedit.modifyAttribute(3, "dummySpeed")

# Change parameter 3 with a non valid value (negative speed)
netedit.modifyAttribute(3, "-13")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "120.5")

# Change parameter 4 with a non valid value (empty priority)
netedit.modifyAttribute(4, "")

# Change parameter 4 with a non valid value (dummy priority)
netedit.modifyAttribute(4, "dummyPriority")

# Change parameter 4 with a non valid value (negative priority)
netedit.modifyAttribute(4, "-6")

# Change parameter 4 with a non valid value (float)
netedit.modifyAttribute(4, "6.4")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "4")

# Change parameter 5 with a non valid value (empty lanes)
netedit.modifyAttribute(5, "")

# Change parameter 5 with a non valid value (dummy lanes)
netedit.modifyAttribute(5, "dummyLanes")

# Change parameter 5 with a non valid value (negative lanes)
netedit.modifyAttribute(5, "-6")

# Change parameter 5 with a non valid value (float)
netedit.modifyAttribute(5, "-3.5")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "4")

# recompute
netedit.rebuildNetwork()

# Value type will not be checked

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
netedit.modifyAttribute(10, "emergency authority army vip passenger hov bus coach tram rail_urban rail rail_electric motorcycle moped pedestrian custom1")

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

# CHECK #3157

# Change parameter 19 with a valid value (empty)
netedit.modifyAttribute(19, "")

# Change parameter 19 with a valid value
netedit.modifyAttribute(19, "34,15.5")

# recompute
netedit.rebuildNetwork()

# Check undos and redos
netedit.undo(match, 30)

# recompute
netedit.rebuildNetwork()

netedit.redo(match, 30)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
