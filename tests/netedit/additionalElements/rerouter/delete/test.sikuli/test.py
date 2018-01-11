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

# select edge child
netedit.selectAdditionalChild(8, 0)

# create rerouter with default parameters
netedit.leftClick(match, 200, 100)

# go to delete mode
netedit.deleteMode()

# remove created rerouter
netedit.leftClick(match, 200, 100)

# remove loaded rerouter
netedit.leftClick(match, 300, 100)

# remove edge of rerouter
netedit.leftClick(match, 300, 200)

# Check undo
netedit.undo(match, 3)

# Change to delete
netedit.deleteMode()

# disble 'Automatically delete additionals'
netedit.changeAutomaticallyDeleteAdditionals(match)

# try to delete edge related to the second loaded rerouter (musn't be allowed)
netedit.leftClick(match, 300, 200)

# wait warning
netedit.waitAutomaticallyDeleteAdditionalsWarning()

# check redo
netedit.redo(match, 3)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
