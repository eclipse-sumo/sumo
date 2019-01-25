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

# go to select mode
netedit.selectMode()

# select first polygon
netedit.leftClick(referencePosition, 100, 50)

# select second polygon
netedit.leftClick(referencePosition, 300, 50)

# go to inspect mode
netedit.inspectMode()

# inspect first polygon
netedit.leftClick(referencePosition, 100, 50)

# Change parameter 8 with a non valid value (dummy)
netedit.modifyAttribute(8, "dummyAngle")

# Change parameter 8 with a valid value (negative)
netedit.modifyAttribute(8, "-5")

# Change parameter 8 with a valid value (> 360)
netedit.modifyAttribute(8, "365")

# Change parameter 8 with a valid value (> 360)
netedit.modifyAttribute(8, "10")

# Check undos and redos
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save shapes
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
