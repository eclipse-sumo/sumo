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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select vaporizer
netedit.changeElement("vaporizer")

# create vaporizer
netedit.leftClick(referencePosition, 250, 120)

# recompute (needed for vertical position)
netedit.rebuildNetwork()

# recompute (needed for vertical position)
netedit.rebuildNetwork()

# go to inspect mode
netedit.inspectMode()

# inspect first vaporizer
netedit.leftClick(referencePosition, 310, 220)

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummy", False)

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-10", False)

# Change parameter 2 with a non valid value (minor than startTime)
netedit.modifyAttribute(2, "2", False)

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "20", False)

# click over an empty area
netedit.leftClick(referencePosition, 0, 0)

# Check undos and redos
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
