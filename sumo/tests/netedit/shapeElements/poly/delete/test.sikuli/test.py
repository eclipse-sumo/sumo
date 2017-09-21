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

# go to shape mode
netedit.shapeMode()

# go to poly mode and select poly
netedit.changeShape("poly")

# create polygon
netedit.createSquaredPoly(match, 100, 50, 100, True)

# go to delete mode
netedit.deleteMode()

# delete created polygon
netedit.leftClick(match, 100, 50)

# try to delete second polygon (not possible due isn't fill)
netedit.leftClick(match, 175, 25)

# delete loaded filled polygon
netedit.leftClick(match, 275, 25)

# Check undos and redos
netedit.undo(match, 3)
netedit.redo(match, 3)

# save shapes
netedit.saveShapes()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
