#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

python script used by sikulix for testing netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR/TS, Germany

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, False)

# go to select mode
netedit.selectMode()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("dummyElement")
# set elementClass
type(Key.TAB)
# abort current selection
netedit.abortSelection()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("Net Element")
# jump to element
for x in range(0, 2):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementType
paste("dummyType")
# type tab to set elementType
type(Key.TAB)
# abort current selection
netedit.abortSelection()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("Net Element")
# jump to element
for x in range(0, 2):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementType
paste("junction")
# jump to attribute
for x in range(0, 2):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new attribute
paste("dummyAttribute")
# type enter to set attribute
type(Key.TAB)
# abort current selection
netedit.abortSelection()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.selectItems("Net Element", "junction", "id", "gneJ0")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
