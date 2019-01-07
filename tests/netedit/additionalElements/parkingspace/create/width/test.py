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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeAdditional("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# select space
netedit.changeAdditional("space")

# set invalid width (dummy)
netedit.selectAdditionalChild(8, 0)
netedit.modifyAdditionalDefaultValue(2, "dummyWidth")

# try to create area
netedit.leftClick(referencePosition, 280, 100)

# set invalid width (empty)
netedit.modifyAdditionalDefaultValue(2, "")

# try to create area
netedit.leftClick(referencePosition, 280, 100)

# set invalid width (negative)
netedit.modifyAdditionalDefaultValue(2, "-3")

# try to create area
netedit.leftClick(referencePosition, 280, 100)

# set valid width
netedit.modifyAdditionalDefaultValue(2, "2.5")

# create area
netedit.leftClick(referencePosition, 280, 100)

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
