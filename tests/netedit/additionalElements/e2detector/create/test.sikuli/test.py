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

# select E2
netedit.changeAdditional("e2Detector")

# create E2 with default parameters
netedit.leftClick(match, 125, 250)

# set invalid  lenght
netedit.modifyAdditionalDefaultValue(2, "-12")

# try to create E2 with different lenght
netedit.leftClick(match, 250, 250)

# set valid lenght
netedit.modifyAdditionalDefaultValue(2, "5")

# create E2 with different lenght
netedit.leftClick(match, 250, 250)

# set invalid frequency
netedit.modifyAdditionalDefaultValue(3, "-30")

# try to create a E2 with different frequency
netedit.leftClick(match, 350, 250)

# set valid frequency
netedit.modifyAdditionalDefaultValue(3, "120")

# create E2 with different frequency
netedit.leftClick(match, 350, 250)

# Change cont
netedit.modifyAdditionalDefaultBoolValue(4)

# create E2 with different cont
netedit.leftClick(match, 450, 250)

# set invalid time threshold
netedit.modifyAdditionalDefaultValue(5, "-12")

# create E2 with different time threshold
netedit.leftClick(match, 150, 220)

# set valid time threshold
netedit.modifyAdditionalDefaultValue(5, "10")

# create E2 with different time threshold
netedit.leftClick(match, 150, 220)

# set invalid speed threshold
netedit.modifyAdditionalDefaultValue(6, "-14")

# try to create E2 with different speed threshold
netedit.leftClick(match, 250, 220)

# set valid speed threshold
netedit.modifyAdditionalDefaultValue(6, "2.5")

# create E2 with different speed threshold
netedit.leftClick(match, 250, 220)

# set invalid jam threshold
netedit.modifyAdditionalDefaultValue(7, "-30")

# try to create E2 with different jam threshold
netedit.leftClick(match, 350, 220)

# set valid jam threshold
netedit.modifyAdditionalDefaultValue(7, "15.5")

# create E2 with different jam threshold
netedit.leftClick(match, 350, 220)

# Change friendlyPos
netedit.modifyAdditionalDefaultBoolValue(8)

# create E2 with different friendlyPos
netedit.leftClick(match, 450, 220)

# Check undo redo
netedit.undo(match, 8)
netedit.redo(match, 8)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
