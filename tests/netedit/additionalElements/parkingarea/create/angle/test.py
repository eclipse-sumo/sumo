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

# set invalid angle (dummy)
netedit.modifyAdditionalDefaultValue(8, "dummyHeight")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 220, 250)

# set invalid angle (empty)
netedit.modifyAdditionalDefaultValue(8, "")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 220, 250)

# set valid angle (negative)
netedit.modifyAdditionalDefaultValue(8, "-4")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 220, 250)

# set valid angle (>360)
netedit.modifyAdditionalDefaultValue(8, "500")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 220, 250)

# set valid angle
netedit.modifyAdditionalDefaultValue(8, "120")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 220, 250)

# Check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
