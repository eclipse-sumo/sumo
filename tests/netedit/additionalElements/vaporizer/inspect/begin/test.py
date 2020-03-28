#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

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
netedit.leftClick(referencePosition, 250, 130)

# recompute (needed for vertical position)
netedit.rebuildNetwork()

# recompute (needed for vertical position)
netedit.rebuildNetwork()

# go to inspect mode
netedit.inspectMode()

# inspect first vaporizer
netedit.leftClick(referencePosition, 310, 220)

# Change parameter 1 with a non valid value (dummy)
netedit.modifyAttribute(1, "dummy", True)

# Change parameter 1 with a non valid value (negative)
netedit.modifyAttribute(1, "-10", True)

# Change parameter 1 with a non valid value (greather than end)
netedit.modifyAttribute(1, "50", True)

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "5", True)

# click over an empty area
netedit.leftClick(referencePosition, 0, 0)

# Check undos and redos
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
