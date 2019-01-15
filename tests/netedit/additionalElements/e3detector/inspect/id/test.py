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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# apply zoom
netedit.setZoom("25", "25", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(referencePosition, 250, 100)

# create E3 2 (for check duplicated ID)
netedit.leftClick(referencePosition, 450, 100)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector E3 (for saving)
netedit.leftClick(referencePosition, 250, 100)
netedit.leftClick(referencePosition, 250, 200)
netedit.leftClick(referencePosition, 450, 100)
netedit.leftClick(referencePosition, 450, 200)

# select entry detector
netedit.changeAdditional("detExit")

# Create Exit detector E3 (for saving)
netedit.leftClick(referencePosition, 250, 100)
netedit.leftClick(referencePosition, 250, 450)
netedit.leftClick(referencePosition, 450, 100)
netedit.leftClick(referencePosition, 450, 450)

# go to inspect mode
netedit.inspectMode()

# inspect first E3
netedit.leftClick(referencePosition, 250, 100)

# Change parameter id with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "e3Detector_1")

# Change parameter id with a non valid value (invalid characters)
netedit.modifyAttribute(0, ";;;;;;;;;;;;;;;;;")

# Change parameter id with a valid value (with spaces)
netedit.modifyAttribute(0, "Id with spaces")

# Change parameter id with a valid value
netedit.modifyAttribute(0, "correctID")

# Check undos and redos
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
