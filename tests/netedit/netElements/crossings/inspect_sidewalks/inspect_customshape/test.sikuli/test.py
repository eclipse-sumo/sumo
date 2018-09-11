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

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.setZoom("50", "50", "50")

# go to inspect mode
netedit.inspectMode()

# inspect first crossing
netedit.leftClick(referencePosition, 250, 225)

# Change shape with a non valid value
netedit.modifyAttribute(5, "dummyShape")

# Change shape with a valid value
netedit.modifyAttribute(5, "42.60,56.52 48.25,55.65 51.97,53.13 51.86,49.56 49.29,45.45 42.87,43.86")

# rebuild network
netedit.rebuildNetwork()

# Check undos
netedit.undo(referencePosition, 1)

# rebuild network
netedit.rebuildNetwork()

# Check redos
netedit.redo(referencePosition, 1)

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
