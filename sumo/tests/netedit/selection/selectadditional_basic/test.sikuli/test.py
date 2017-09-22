#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

python script used by sikulix for testing netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""
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

# select all busStops with lanes that contains "E1_0" and remove it
netedit.selectItems("Additional", "busStop", "lane", "E0_0")
netedit.deleteSelectedItems()

# select all busStops with id "busStop_IDToRemove1" and remove it
netedit.selectItems("Additional", "busStop", "id", "=busStop_IDToRemove1")
netedit.deleteSelectedItems()

# select all busStops with id "busStop_IDToRemove2" and remove it
netedit.selectItems("Additional", "busStop", "id", "=busStop_IDToRemove2")
netedit.deleteSelectedItems()

# select all busStops with startPos greather than 15
netedit.selectItems("Additional", "busStop", "startPos", ">14")

# change to remove mode
netedit.modificationModeRemove()

# select all busStops with startPos greather than 25
netedit.selectItems("Additional", "busStop", "startPos", ">24")

# change to add mode and remove selected busStops
netedit.modificationModeAdd()
netedit.deleteSelectedItems()

# select all busStops with endPos minor that 30 and delete it
netedit.selectItems("Additional", "busStop", "endPos", "<36")
netedit.deleteSelectedItems()

# select all busStops with lines that contains "lineToRemove" and remove it
netedit.selectItems("Additional", "busStop", "lines", "lineToRemove")
netedit.deleteSelectedItems()

# select all busStops with lines that contains "linetoRemove" and remove it
netedit.selectItems("Additional", "busStop", "lines", "linetoRemove")
netedit.deleteSelectedItems()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
