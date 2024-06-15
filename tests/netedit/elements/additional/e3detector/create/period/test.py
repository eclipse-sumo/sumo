#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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

# select E3
netedit.changeElement("entryExitDetector")

# set invalid frequency
netedit.changeDefaultValue(netedit.attrs.E3.create.period, "dummyFreq")

# try to create E3 with invalid frequency
netedit.leftClick(referencePosition, netedit.positions.additionalElements.squaredAdditionalA.x,
                  netedit.positions.additionalElements.squaredAdditionalA.y)

# set invalid frequency (negative)
netedit.changeDefaultValue(netedit.attrs.E3.create.period, "-30")

# try to create E3 with different frequency
netedit.leftClick(referencePosition, netedit.positions.additionalElements.squaredAdditionalA.x,
                  netedit.positions.additionalElements.squaredAdditionalA.y)

# set valid frequency
netedit.changeDefaultValue(netedit.attrs.E3.create.period, "150.5")

# create E3 with valid frequency
netedit.leftClick(referencePosition, netedit.positions.additionalElements.squaredAdditionalA.x,
                  netedit.positions.additionalElements.squaredAdditionalA.y)

# select entry detector
netedit.changeElement("detExit")

# Create entry detector with default value
netedit.leftClick(referencePosition, netedit.positions.additionalElements.squaredAdditionalA.x,
                  netedit.positions.additionalElements.squaredAdditionalA.y)
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge0.x,
                  netedit.positions.additionalElements.centralEdge0.y)

# select exit detector
netedit.changeElement("detEntry")

netedit.leftClick(referencePosition, netedit.positions.additionalElements.squaredAdditionalA.x,
                  netedit.positions.additionalElements.squaredAdditionalA.y)
netedit.leftClick(referencePosition, netedit.positions.additionalElements.centralEdge2.x,
                  netedit.positions.additionalElements.centralEdge2.y)

# Check undo redo
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
