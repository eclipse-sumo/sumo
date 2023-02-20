#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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
import time

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# Change to create mode
netedit.createEdgeMode()

# Create two nodes
netedit.leftClick(referencePosition, 115, 240)
netedit.leftClick(referencePosition, 536, 240)

# Check undo and redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save network using hotkey
netedit.typeTwoKeys('ctrl', 's')

# jump to filename TextField
netedit.typeTwoKeys('alt', 'f')
netedit.pasteIntoTextField(os.environ.get("TEXTTEST_SANDBOX", os.getcwd()))
netedit.typeEnter()
netedit.pasteIntoTextField("net")
netedit.typeEnter()

# wait for loading
time.sleep(1)

# quit netedit
netedit.quit(neteditProcess)
