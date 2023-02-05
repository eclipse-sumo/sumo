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
import pyautogui
import time

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to TLS mode
netedit.selectTLSMode()

# select junction
netedit.leftClick(referencePosition, 320, 220)

# create TLS
netedit.createTLS()

# obtain clicked position
clickedPosition = [referencePosition[0] + 227, referencePosition[1] + 181]

# click relative to offset
pyautogui.rightClick(clickedPosition)

# place cursor over first operation
for _ in range(7):
    # wait before every down
    time.sleep(0.3)
# type down keys
    pyautogui.hotkey('down')

# type right key for the second menu
netedit.typeSpace()

# focus on frame
netedit.focusOnFrame()

# place cursor over first operation
for _ in range(3):
    # wait before every down
    time.sleep(0.3)
# type down keys
    netedit.typeTab()

# type right key for the second menu
netedit.typeSpace()

# create TLS
netedit.inspectMode()

# Check undo
netedit.undo(referencePosition, 1)

# Check redo
netedit.redo(referencePosition, 1)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
