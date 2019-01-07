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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeAdditional("e2Detector")

# set invalid  lenght (dummy)
netedit.modifyAdditionalDefaultValue(2, "dummyLenght")

# try to create E2 with invalid lenght
netedit.leftClick(referencePosition, 250, 250)

# set invalid  lenght (0)
netedit.modifyAdditionalDefaultValue(2, "0")

# try to create E2 with invalid lenght
netedit.leftClick(referencePosition, 250, 250)

# set invalid  lenght (negative)
netedit.modifyAdditionalDefaultValue(2, "-12")

# try to create E2 with invalid lenght
netedit.leftClick(referencePosition, 250, 250)

# set valid lenght
netedit.modifyAdditionalDefaultValue(2, "5")

# create E2 with valid lenght
netedit.leftClick(referencePosition, 250, 250)

# Check undo redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
