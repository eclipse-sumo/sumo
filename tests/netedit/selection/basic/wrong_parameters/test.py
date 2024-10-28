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

# go to select mode
netedit.selectMode()

# try to select node gneJ0 and delete it (Error in element set)
netedit.focusOnFrame()
for x in range(0, 8):
    netedit.typeTab()

# paste the new elementClass
netedit.pasteIntoTextField("dummyElement")

# set elementClass
netedit.typeEnter()

# try to select node gneJ0 and delete it (Error in element type)
netedit.focusOnFrame()
for x in range(0, 8):
    netedit.typeTab()

# paste the new elementClass
netedit.pasteIntoTextField("Network")

# jump to element
for x in range(0, 3):
    netedit.typeTab()

# paste the new elementType
netedit.pasteIntoTextField("dummyType")

# type tab to set elementType
netedit.typeEnter()

# try to select node gneJ0 and delete it (Error in set)
netedit.focusOnFrame()
for x in range(0, 8):
    netedit.typeTab()

# paste the new elementClass
netedit.pasteIntoTextField("Network")

# jump to element
for x in range(0, 3):
    netedit.typeTab()

# paste the new elementType
netedit.pasteIntoTextField("junction")

# jump to attribute
for x in range(0, 2):
    netedit.typeTab()

# paste the new attribute
netedit.pasteIntoTextField("dummyAttribute")

# type enter to set attribute
netedit.typeEnter()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.selectItems("Network", "junction", "id", "J0")
netedit.deleteSelectedItems()

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
