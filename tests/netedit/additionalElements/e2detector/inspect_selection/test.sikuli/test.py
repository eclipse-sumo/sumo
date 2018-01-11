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

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect E2s
netedit.leftClick(match, 150, 250)

# Set invalid length
netedit.modifyAttribute(0, "-12")

# Set valid length
netedit.modifyAttribute(0, "7")

# Set invalid freq
netedit.modifyAttribute(1, "-30")

# Set valid freq
netedit.modifyAttribute(1, "50")

# Set cont
netedit.modifyBoolAttribute(2)

# Set invalid time speed treshold
netedit.modifyAttribute(3, "-5")

# Set valid time speed treshold
netedit.modifyAttribute(3, "11.3")

# Set invalid speed speed treshold
netedit.modifyAttribute(4, "-3")

# Set valid speed speed treshold
netedit.modifyAttribute(4, "4")

# Set invalid jam speed treshold
netedit.modifyAttribute(5, "-6")

# Set valid jam speed treshold
netedit.modifyAttribute(5, "8.5")

# Set friendlyPos
netedit.modifyBoolAttribute(6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
