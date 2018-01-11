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

# go to additional mode
netedit.additionalMode()

# select E1
netedit.changeAdditional("e1Detector")

# create E1 with default parameters
netedit.leftClick(match, 200, 250)

# set a invalid  frequency
netedit.modifyAdditionalDefaultValue(2, "-30")

# try to create E1 with different frequency
netedit.leftClick(match, 250, 250)

# set a valid frequency
netedit.modifyAdditionalDefaultValue(2, "150")

# create E1 with different frequency
netedit.leftClick(match, 250, 250)

# set different vehicle type
netedit.modifyAdditionalDefaultValue(3, "private passenger taxi bus")

# create E1 with different split by type
netedit.leftClick(match, 300, 250)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(4)

# create E1 with different split by type
netedit.leftClick(match, 350, 250)

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
