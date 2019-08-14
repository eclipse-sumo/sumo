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

# select parkingArea
netedit.changeElement("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 180)

# select space
netedit.changeElement("space")

# create space
netedit.selectAdditionalChild(8, 0)
netedit.leftClick(referencePosition, 250, 320)

# go to inspect mode
netedit.inspectMode()

# inspect space
netedit.leftClick(referencePosition, 250, 300)

# Change parameter 3 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyAngle", False)

# Change parameter 3 with a valid value (negative)
netedit.modifyAttribute(3, "-6", False)

# Change parameter 3 with a valid value >360
netedit.modifyAttribute(3, "500", False)

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "32.5", False)

# Check undos and redos
netedit.undo(referencePosition, 5)
netedit.redo(referencePosition, 5)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
