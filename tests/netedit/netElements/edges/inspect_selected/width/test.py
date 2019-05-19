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

# recompute
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# select first edge
netedit.leftClick(referencePosition, 250, 180)

# select second edge
netedit.leftClick(referencePosition, 250, 100)

# go to inspect mode
netedit.inspectMode()

# inspect selected edges
netedit.leftClick(referencePosition, 250, 180)

# Change parameter 12 with a non valid value (dummy)
netedit.modifyAttribute(12, "dummyWidth", False)

# Change parameter 12 with a non valid value (empty)
netedit.modifyAttribute(12, "", False)

# Change parameter 12 with a non valid value (negative)
netedit.modifyAttribute(12, "-2", False)

# Change parameter 12 with a valid value (default)
netedit.modifyAttribute(12, "default", False)

# Change parameter 12 with a valid value (default)
netedit.modifyAttribute(12, "4", False)

# recompute
netedit.rebuildNetwork()

# Check undos
netedit.undo(referencePosition, 1)

# recompute
netedit.rebuildNetwork()

# check redos
netedit.redo(referencePosition, 1)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
