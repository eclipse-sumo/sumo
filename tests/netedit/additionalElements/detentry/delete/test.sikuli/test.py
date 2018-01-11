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

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(match, 100, 50)

# create E3 2
netedit.leftClick(match, 200, 50)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector for E3 3
netedit.selectAdditionalChild(6, 2)
netedit.leftClick(match, 75, 200)

# Create Entry detectors for E3 4
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 250, 200)

# Change to delete
netedit.deleteMode()

# delete created E3 1
netedit.leftClick(match, 100, 50)

# delete Entry detector of E3 2 (E3 will not be removed)
netedit.leftClick(match, 250, 200)

# delete loaded E3 1
netedit.leftClick(match, 400, 50)

# delete Entry detector of loaded E3 2 (E3 will not be removed)
netedit.leftClick(match, 550, 200)

# delete lane with the last loaded entry
netedit.leftClick(match, 400, 300)

# Check undo
netedit.undo(match, 5)

# Change to delete
netedit.deleteMode()

# disble 'Automatically delete additionals'
netedit.changeAutomaticallyDeleteAdditionals(match)

# try to delete lane with the second loaded entry (doesn't allowed)
netedit.leftClick(match, 400, 300)

# wait warning
netedit.waitAutomaticallyDeleteAdditionalsWarning()

# check redo
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
