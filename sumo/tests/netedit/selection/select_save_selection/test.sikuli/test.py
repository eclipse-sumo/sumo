#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

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

# select all edges that allow passengers
netedit.selectItems("Net Element", "edge", "allow", "passenger")

# save selection
netedit.focusOnFrame()
for x in range(0, 16):
    netedit.typeTab()
netedit.typeSpace()
# jump to filename textfield
netedit.typeTwoKeys("f", Key.ALT)
filename = os.path.join(netedit.textTestSandBox, "selection.txt")
netedit.pasteIntoTextField(filename)
netedit.typeEnter()

# quit netedit
netedit.quit(neteditProcess, True)
