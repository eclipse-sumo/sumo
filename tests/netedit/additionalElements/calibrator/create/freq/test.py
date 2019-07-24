#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select calibrator
netedit.changeElement("calibrator")

# change frequency with an invalid value (dummy)
netedit.changeDefaultValue(3, "dummyFreq")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 240, 250)

# change frequency with an invalid value (negative)
netedit.changeDefaultValue(3, "-30")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 240, 250)

# change frequency with a valid value
netedit.changeDefaultValue(3, "250.5")

# create calibrator with a valid parameter
netedit.leftClick(referencePosition, 240, 250)

# Check undo redo
netedit.undo(referencePosition, 1)
netedit.redo(referencePosition, 1)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
