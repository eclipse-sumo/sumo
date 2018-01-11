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

# select vaporizer
netedit.changeAdditional("vaporizer")

# create vaporizer (camera will be moved)
netedit.leftClick(match, 250, 220)

# create another vaporizer with the same default attributes (camera will be moved)
netedit.leftClick(match, 250, 220)

# set invalid start
netedit.modifyAdditionalDefaultValue(2, "-12")

# try to create vaporizer
netedit.leftClick(match, 250, 220)

# set valid start
netedit.modifyAdditionalDefaultValue(2, "10")

# create vaporizer (camera will be moved)
netedit.leftClick(match, 250, 220)

# set invalid end
netedit.modifyAdditionalDefaultValue(3, "-20")

# try to create create vaporizer
netedit.leftClick(match, 250, 220)

# set valid end
netedit.modifyAdditionalDefaultValue(3, "20")

# create vaporizer
netedit.leftClick(match, 250, 220)

# change default start (Invalid, end > startTime)
netedit.modifyAdditionalDefaultValue(2, "50")

# try to create invalid vaporizer (show warning)
netedit.leftClick(match, 250, 220)

# change default end (valid))
netedit.modifyAdditionalDefaultValue(2, "100")

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
