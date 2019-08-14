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

# Rebuild network
netedit.rebuildNetwork()

# show connections
netedit.changeEditMode('3')

# inspect connection
netedit.leftClick(referencePosition, 250, 138)

# Change linkIndex with an invalid value
netedit.modifyAttribute(5, "dummyLinkIndex", True)

# Change linkIndex with an valid value
netedit.modifyAttribute(5, "-2", True)

# Change linkIndex with an valid value
netedit.modifyAttribute(5, "0", True)

# Change linkIndex with an valid value (but big)
netedit.modifyAttribute(5, "600", True)

# Change linkIndex with an valid value
netedit.modifyAttribute(5, "12", True)

# rebuild
netedit.rebuildNetwork()

# Check undo
netedit.undo(referencePosition, 5)

# rebuild
netedit.rebuildNetwork()

# Check redo
netedit.redo(referencePosition, 5)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
