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
netedit.leftClick(referencePosition, 150, 290)

# Change parameter 0 with a non valid value (dummy)
netedit.modifyAttribute(0, "%$%$$&&$$")

# Change parameter 0 with a non valid value (double)
netedit.modifyAttribute(0, "Parking Area Name")

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummyRoadSideCapacity")

# Change parameter 1 with a non valid value (double)
netedit.modifyAttribute(1, "2.3")

# Change parameter 1 with a non valid value (negative)
netedit.modifyAttribute(1, "-5")

# Change parameter 1 with a non valid value (negative)
netedit.modifyAttribute(1, "7")

# Change parameter 2 with a different value
netedit.modifyAttribute(2, "true")

# Change parameter 3 with a non valid value (dummy)
netedit.modifyAttribute(3, "dummyWidth")

# Change parameter 3 with a non valid value (negative)
netedit.modifyAttribute(3, "-5")

# Change parameter 3 with a non valid value (0)
netedit.modifyAttribute(3, "0")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "2.3")

# Change parameter 4 with a non valid value (dummy)
netedit.modifyAttribute(4, "dummyHeight")

# Change parameter 4 with a non valid value (negative)
netedit.modifyAttribute(4, "-6")

# Change parameter 4 with a non valid value (0)
netedit.modifyAttribute(4, "0")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "3.4")

# Change parameter 5 with a non valid value (dummy)
netedit.modifyAttribute(5, "dummyAngle")

# Change parameter 5 with a valid value (negative)
netedit.modifyAttribute(5, "-6")

# Change parameter 5 with a valid value >360
netedit.modifyAttribute(5, "500")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "32.5")

# Change parameter 5 with a different value
netedit.modifyBoolAttribute(9)

# Check undos and redos
netedit.undo(referencePosition, 17)
netedit.redo(referencePosition, 17)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
