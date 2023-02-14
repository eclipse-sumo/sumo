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

# select containerStop
netedit.changeElement("containerStop")

# set invalid length (dummy)
netedit.changeDefaultValue(netedit.attrs.containerStop.create.length, "dummyLenght")

# try to create containerstop with invalid length
netedit.leftClick(referencePosition, 430, 256)

# set invalid length (negative)
netedit.changeDefaultValue(netedit.attrs.containerStop.create.length, "-20")

# try to create containerstop with invalid length
netedit.leftClick(referencePosition, 497, 257)

# Change length
netedit.changeDefaultValue(netedit.attrs.containerStop.create.length, "5")

# create containerStop in mode "reference left"
netedit.leftClick(referencePosition, 544, 257)

# change reference to right
netedit.changeDefaultValue(netedit.attrs.containerStop.create.references, "reference right")

# create containerStop in mode "reference right"
netedit.leftClick(referencePosition, 406, 257)

# change reference to center
netedit.changeDefaultValue(netedit.attrs.containerStop.create.references, "reference center")

# create containerStop in mode "reference center"
netedit.leftClick(referencePosition, 496, 257)

# Check undo redo
netedit.undo(referencePosition, 3)
netedit.redo(referencePosition, 3)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
