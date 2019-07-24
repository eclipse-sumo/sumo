#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
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
netedit.changeElement("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# select space
netedit.changeElement("space")

# set invalid lenght (dummy)
netedit.selectAdditionalChild(8, 0)
netedit.changeDefaultValue(3, "dummyLenght")

# try to create area
netedit.leftClick(referencePosition, 290, 100)

# set invalid height (empty)
netedit.changeDefaultValue(3, "")

# try to create area
netedit.leftClick(referencePosition, 290, 100)

# set invalid height (negative)
netedit.changeDefaultValue(3, "-4")

# try to create area
netedit.leftClick(referencePosition, 290, 100)

# set valid height
netedit.changeDefaultValue(3, "3.1")

# create area
netedit.leftClick(referencePosition, 290, 100)

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
