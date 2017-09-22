#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

python script used by sikulix for testing netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""
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
