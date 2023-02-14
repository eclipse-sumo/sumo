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

# change reference to center
netedit.changeDefaultValue(netedit.attrs.containerStop.create.references, "reference center")

# create containerStop in mode "reference center"
netedit.leftClick(referencePosition, 430, 256)

# go to inspect mode
netedit.inspectMode()

# inspect first containerStop
netedit.leftClick(referencePosition, 430, 280)

# change color using dialog
netedit.modifyColorAttribute(netedit.attrs.containerStop.inspect.colorButton, 5, True)

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(netedit.attrs.containerStop.inspect.color, "dummyColor", True)

# Change parameter 2 with a non valid value (invalid format)
netedit.modifyAttribute(netedit.attrs.containerStop.inspect.color, "255,255,500", True)

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(netedit.attrs.containerStop.inspect.color, "blue", True)

# Change parameter 2 with a valid value (valid format)
netedit.modifyAttribute(netedit.attrs.containerStop.inspect.color, "125,60,200", True)

# Check undos and redos
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
