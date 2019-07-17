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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeAdditional("busStop")

# create busStop in mode "reference left"
netedit.leftClick(referencePosition, 250, 260)

# Change to delete
netedit.deleteMode()

# delete created busStop
netedit.leftClick(referencePosition, 260, 270)

# delete first loaded busStop
netedit.leftClick(referencePosition, 450, 270)

# delete lane with the second loaded busStop
netedit.leftClick(referencePosition, 200, 220)

# Check undo
netedit.undo(referencePosition, 3)

# Change to delete
netedit.deleteMode()

# disble 'Automatically delete additionals'
netedit.changeAutomaticallyDeleteAdditionals(referencePosition)

# try to delete lane with the second loaded busStop (doesn't allowed)
netedit.leftClick(referencePosition, 200, 220)

# wait warning
netedit.waitDeleteWarning()

# check redo
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
