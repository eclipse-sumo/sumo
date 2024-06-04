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

# apply zoom
netedit.setZoom("0", "5", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeElement("entryExitDetector")

# create E3 with default parameters
netedit.leftClick(referencePosition, 376, 83)

# create second E3 with default parameters
netedit.leftClick(referencePosition, 194, 83)

# select entry detector
netedit.changeElement("detEntry")

# Create entry detector with default value
netedit.leftClick(referencePosition, 376, 83)
netedit.leftClick(referencePosition, 194, 321)

# select exit detector
netedit.changeElement("detExit")

netedit.leftClick(referencePosition, 376, 83)
netedit.leftClick(referencePosition, 619, 321)

netedit.leftClick(referencePosition, 194, 83)
netedit.leftClick(referencePosition, 619, 236)

# go to inspect mode
netedit.inspectMode()

# inspect
netedit.leftClick(referencePosition, 194, 326)

# Change Netedit parameter 1 with a non valid value (Invalid E3 ID)
netedit.modifyAttribute(netedit.attrs.entryExit.inspect.parent, "invalidE3", True)

# Change Netedit parameter 2 with a non valid value (Invalid E3 ID)
netedit.modifyAttribute(netedit.attrs.entryExit.inspect.parent, "e3_1", True)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
