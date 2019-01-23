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

# first rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# select manually the junction center
netedit.leftClick(referencePosition, 320, 220)

# use a rectangle to check add mode
netedit.selectionRectangle(referencePosition, 250, 150, 400, 300)

# clear selection
netedit.selectionClear()

# Select "remove" mode
netedit.modificationModeRemove()

# select manually the junction center
netedit.leftClick(referencePosition, 320, 220)

# use a rectangle to check remove mode
netedit.selectionRectangle(referencePosition, 250, 150, 400, 300)

# Select "keep" mode
netedit.modificationModeKeep()

# invert selection
netedit.selectionInvert()

# use a rectangle to check keep mode
netedit.selectionRectangle(referencePosition, 250, 150, 400, 300)

# Select "replace" mode
netedit.modificationModeReplace()

# clear selection
netedit.selectionInvert()

# use a rectangle to check replace mode
netedit.selectionRectangle(referencePosition, 250, 150, 400, 300)

# check undo and redo
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# save additionals and shapes
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
