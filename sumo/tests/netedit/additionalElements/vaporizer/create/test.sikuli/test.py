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
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select vaporizer
netedit.changeAdditional("vaporizer")

# create vaporizer (camera will be moved)
netedit.leftClick(match, 250, 220)

# create another vaporizer with the same default attributes (camera will be moved)
netedit.leftClick(match, 250, 220)

# set invalid start
netedit.modifyAdditionalDefaultValue(2, "-12")

# try to create vaporizer
netedit.leftClick(match, 250, 220)

# set valid start
netedit.modifyAdditionalDefaultValue(2, "10")

# create vaporizer (camera will be moved)
netedit.leftClick(match, 250, 220)

# set invalid end
netedit.modifyAdditionalDefaultValue(3, "-20")

# try to create create vaporizer
netedit.leftClick(match, 250, 220)

# set valid end
netedit.modifyAdditionalDefaultValue(3, "20")

# create vaporizer
netedit.leftClick(match, 250, 220)

# change default start (Invalid, end > startTime)
netedit.modifyAdditionalDefaultValue(2, "50")

# try to create invalid vaporizer (show warning)
netedit.leftClick(match, 250, 220)

# change default end (valid))
netedit.modifyAdditionalDefaultValue(2, "100")

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
