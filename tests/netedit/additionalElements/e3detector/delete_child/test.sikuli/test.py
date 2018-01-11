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

# create E3
netedit.leftClick(match, 100, 50)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 100, 200)

# select Exit detector
netedit.changeAdditional("detExit")

# Create Exit detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 250, 200)

# Undo creation of E3, Entry and Exit
netedit.leftClick(match, 0, 0)
netedit.undo(match, 3)

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3
netedit.leftClick(match, 100, 50)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 100, 200)

# select Exit detector
netedit.changeAdditional("detExit")

# Create Exit detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 250, 200)

# Undo creation of Entry and Exit
netedit.leftClick(match, 0, 0)
netedit.undo(match, 2)

# go to additional mode
netedit.additionalMode()

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 100, 200)

# select Exit detector
netedit.changeAdditional("detExit")

# Create Exit detector for E3
netedit.selectAdditionalChild(6, 3)
netedit.leftClick(match, 250, 200)

# Change to delete
netedit.deleteMode()

# Delete the four Entry/exits
netedit.leftClick(match, 100, 200)
netedit.leftClick(match, 250, 200)
netedit.leftClick(match, 450, 200)
netedit.leftClick(match, 600, 200)

# Check undo redo
netedit.undo(match, 7)
netedit.redo(match, 7)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
