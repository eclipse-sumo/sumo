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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# select node 1
netedit.leftClick(match, 175, 220)

# select node 2
netedit.leftClick(match, 315, 220)

# select node 2
netedit.leftClick(match, 465, 220)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# Undo joining
netedit.undo(match, 1)

# rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# unselect node 2
netedit.leftClick(match, 315, 220)

# join selected junctions
netedit.joinSelectedJunctions()

# abort joining
netedit.waitQuestion('n')

# join selected junctions
netedit.joinSelectedJunctions()

# allow joining
netedit.waitQuestion('y')

# rebuild network
netedit.rebuildNetwork()

# Undo joining
netedit.undo(match, 1)

# rebuild network
netedit.rebuildNetwork()

# redo joining
netedit.redo(match, 1)

# rebuild network
netedit.rebuildNetwork()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
