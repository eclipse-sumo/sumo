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

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect parking areas
netedit.leftClick(referencePosition, 230, 130)

# Change parameter 0 with a non valid value (dummy)
netedit.modifyAttribute(0, "dummyZ")

# Change parameter 0 with a non valid value (empty)
netedit.modifyAttribute(0, "")

# Change parameter 0 with a valid value (-negative)
netedit.modifyAttribute(0, "-4.25")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "1.00")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyWidth")

# Change parameter 1 with a non valid value (negative)
netedit.modifyAttribute(1, "-5")

# Change parameter 1 with a non valid value (0)
netedit.modifyAttribute(1, "0")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "2.3")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummyHeight")

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-6")

# Change parameter 2 with a non valid value (0)
netedit.modifyAttribute(2, "0")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "3.4")

# Change parameter 3 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyAngle")

# Change parameter 3 with a valid value (negative)
netedit.modifyAttribute(3, "-6")

# Change parameter 3 with a valid value >360
netedit.modifyAttribute(3, "500")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "32.5")

# Change parameter 5 with a different value
netedit.modifyAttribute(7, "parkingAreaNewParent")

# Change parameter 6 with a different value
netedit.modifyBoolAttribute(8)

# Check undos and redos
netedit.undo(referencePosition, 17)
netedit.redo(referencePosition, 17)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
