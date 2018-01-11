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

# select rerouter
netedit.changeAdditional("rerouter")

# try to create rerouter without edge child (Warning)
netedit.leftClick(match, 100, 100)

# select edge child
netedit.selectAdditionalChild(8, 0)

# create rerouter with default parameters
netedit.leftClick(match, 200, 100)

# set invalid probability
netedit.modifyAdditionalDefaultValue(2, "2")

# try to create rerouter with different frequency
netedit.leftClick(match, 300, 100)

# set valid probability
netedit.modifyAdditionalDefaultValue(2, "0.3")

# create rerouter with different probability
netedit.leftClick(match, 300, 100)

# change off
netedit.modifyAdditionalDefaultBoolValue(3)

# try to create rerouter with different timeTreshold
netedit.leftClick(match, 400, 100)

# Check undo redo
netedit.undo(match, 3)
netedit.redo(match, 3)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
