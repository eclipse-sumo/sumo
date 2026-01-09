#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# go to TLS mode
netedit.changeMode("TLS")

# select junction
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center)

# create TLS
netedit.createTLS()

# click relative to offset
netedit.rightClick(referencePosition, netedit.positions.network.TLS.edit)

# place cursor over first operation
for _ in range(7):
    # wait before every down
    time.sleep(0.3)
# type down keys
    pyautogui.hotkey("down")

# type right key for the second menu
netedit.typeKey("space")

# focus on frame
netedit.focusOnFrame()

# place cursor over first operation
for _ in range(3):
    # wait before every down
    time.sleep(0.3)
# type down keys
    netedit.typeKey("tab")

# type right key for the second menu
netedit.typeKey("space")

# create TLS
netedit.changeMode("inspect")

# Check undo
netedit.undo(referencePosition, 1)

# Check redo
netedit.redo(referencePosition, 1)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
