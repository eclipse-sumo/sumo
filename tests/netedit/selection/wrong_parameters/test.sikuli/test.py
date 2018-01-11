#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to select mode
netedit.selectMode()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    netedit.typeTab()
# paste the new elementClass
netedit.pasteIntoTextField("dummyElement")
# set elementClass
netedit.typeEnter()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    netedit.typeTab()
# paste the new elementClass
netedit.pasteIntoTextField("Net Element")
# jump to element
for x in range(0, 2):
    netedit.typeTab()
# paste the new elementType
netedit.pasteIntoTextField("dummyType")
# type tab to set elementType
netedit.typeEnter()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    netedit.typeTab()
# paste the new elementClass
netedit.pasteIntoTextField("Net Element")
# jump to element
for x in range(0, 2):
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
netedit.selectItems("Net Element", "junction", "id", "gneJ0")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
