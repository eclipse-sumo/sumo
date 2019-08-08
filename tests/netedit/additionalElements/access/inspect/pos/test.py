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

# apply zoom
netedit.setZoom("25", "20", "25")

# go to additional mode
netedit.additionalMode()

# select BusStop
netedit.changeElement("busStop")

# create BusStop with default parameters
netedit.leftClick(referencePosition, 375, 250)

# select Access
netedit.changeElement("access")

# Create Access
netedit.selectAdditionalChild(7, 0)
netedit.leftClick(referencePosition, 200, 280)

# go to inspect mode
netedit.inspectMode()

# delete Access
netedit.leftClick(referencePosition, 208, 280)

# Change parameter pos with a non valid value (dummy position X)
netedit.modifyAttribute(1, "dummy position", True)

# Change parameter pos with a valid value (empty)
netedit.modifyAttribute(1, "", True)

# Change parameter pos with a valid value (negativ)
netedit.modifyAttribute(1, "-1000", True)

# Change parameter pos with a valid value (greather than lane lenght)
netedit.modifyAttribute(1, "1000", True)

# Change parameter pos with a valid value (middle lane)
netedit.modifyAttribute(1, "25", True)

# Check undo redo
netedit.undo(referencePosition, 6)
netedit.redo(referencePosition, 6)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
