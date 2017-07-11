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
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.setZoom("50", "50", "150")

# Change to create edge mode
netedit.createEdgeMode()

# Create an edge in
netedit.leftClick(match, -80, 50)
netedit.leftClick(match, 45, 50)

netedit.leftClick(match, -80, 50)
netedit.leftClick(match, -78, 215)

netedit.leftClick(match, 460, 50)
netedit.leftClick(match, 500, 50)

netedit.leftClick(match, 500, 50)
netedit.leftClick(match, 500, 180)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
