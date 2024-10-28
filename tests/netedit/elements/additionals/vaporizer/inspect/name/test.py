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

# select vaporizer
netedit.changeElement("vaporizer")

# disable center view
netedit.changeDefaultBoolValue(netedit.attrs.vaporizer.create.center)

# create vaporizer (camera will be moved)
netedit.leftClick(referencePosition, netedit.positions.elements.edgeCenter1)

# go to inspect mode
netedit.inspectMode()

# inspect first busStop
netedit.leftClick(referencePosition, netedit.positions.elements.additionals.vaporizer)

# Change parameter 3 with a valid value
netedit.modifyAttribute(netedit.attrs.vaporizer.inspect.name, "%%$$$""", False)

# Change parameter 3 with a valid value
netedit.modifyAttribute(netedit.attrs.vaporizer.inspect.name, "customName", False)

# Check undos and redos
netedit.checkUndoRedo(referencePosition)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
