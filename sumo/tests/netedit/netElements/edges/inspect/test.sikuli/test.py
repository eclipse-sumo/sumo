#!/usr/bin/env python
"""
@file    test.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id: test.py 24005 2017-04-21 12:54:13Z palcraft $

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

# zoom in central node
netedit.setZoom("50", "50", "100")

# rebuild net
netedit.rebuildNetwork()

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(match, 200, 200)

# set invalid ID (duplicated)
netedit.modifyAttribute(0, "6")

# set invalid ID (duplicated)
netedit.modifyAttribute(0, "newID")

# set invalid from (non existent)
netedit.modifyAttribute(1, "dummyFrom")

# set invalid from (to)
netedit.modifyAttribute(1, "C")

# set valid from
netedit.modifyAttribute(1, "B")

# set invalid to (non existent)
netedit.modifyAttribute(1, "dummyTo")

# set invalid to (from)
netedit.modifyAttribute(2, "B")

# set invalid to (the same)
netedit.modifyAttribute(2, "A")


# Check undos
#netedit.undo(match, 2)

# Check redos
#netedit.redo(match, 2)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
