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
