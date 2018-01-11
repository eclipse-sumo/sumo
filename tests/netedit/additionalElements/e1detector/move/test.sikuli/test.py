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

# create E1
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move E1 to left
netedit.moveElement(match, 120, 250, 50, 250)

# move back
netedit.moveElement(match, 50, 250, 120, 250)

# move E1 to right
netedit.moveElement(match, 120, 250, 250, 250)

# move back
netedit.moveElement(match, 250, 250, 120, 250)

# move E1 to left overpassing lane
netedit.moveElement(match, 120, 250, -150, 250)

# move back
netedit.moveElement(match, -100, 250, 120, 250)

# move E1 to right overpassing lane
netedit.moveElement(match, 120, 250, 580, 250)

# move back to another different position of initial
netedit.moveElement(match, 520, 250, 300, 250)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
