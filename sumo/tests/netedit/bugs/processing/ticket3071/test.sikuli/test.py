#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id: test.py 25267 2017-07-19 10:41:16Z behrisch $

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
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# select node 1
netedit.leftClick(match, 175, 220)

# select node 2
netedit.leftClick(match, 315, 220)

# select node 2
netedit.leftClick(match, 465, 220)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# Undo joining
netedit.undo(match, 1)

# rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# unselect node 2
netedit.leftClick(match, 315, 220)

# join selected junctions
netedit.joinSelectedJunctions()

# abort joining
netedit.waitQuestion('n')

# join selected junctions
netedit.joinSelectedJunctions()

# allow joining
netedit.waitQuestion('y')

# rebuild network
netedit.rebuildNetwork()

# Undo joining
netedit.undo(match, 1)

# rebuild network
netedit.rebuildNetwork()

# redo joining
netedit.redo(match, 1)

# rebuild network
netedit.rebuildNetwork()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
