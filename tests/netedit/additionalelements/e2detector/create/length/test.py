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

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select E2
netedit.changeElement("laneAreaDetector")

# set invalid  length (dummy)
netedit.changeDefaultValue(netedit.attrs.E2.create.length, "dummyLenght")

# try to create E2 with invalid length
netedit.leftClick(referencePosition, 427, 253)

# set invalid  length (0)
netedit.changeDefaultValue(netedit.attrs.E2.create.length, "0")

# try to create E2 with invalid length
netedit.leftClick(referencePosition, 427, 253)

# set invalid  length (negative)
netedit.changeDefaultValue(netedit.attrs.E2.create.length, "-12")

# try to create E2 with invalid length
netedit.leftClick(referencePosition, 381, 253)

# set valid length
netedit.changeDefaultValue(netedit.attrs.E2.create.length, "5")

# create E2 with valid length
netedit.leftClick(referencePosition, 427, 253)

# Check undo redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
