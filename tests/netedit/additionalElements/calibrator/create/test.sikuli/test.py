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

# go to additional mode
netedit.additionalMode()

# select calibrator
netedit.changeAdditional("calibrator")

# create calibrator
netedit.leftClick(match, 240, 250)

# change position with an invalid value (dummy)
netedit.modifyAdditionalDefaultValue(2, "dummyValue")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(match, 400, 215)

# change position with an invalid value (negative)
netedit.modifyAdditionalDefaultValue(2, "-5")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(match, 400, 215)

# change position with n valid value
netedit.modifyAdditionalDefaultValue(2, "10")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(match, 400, 215)

# change frequency with an invalid value (dummy)
netedit.modifyAdditionalDefaultValue(3, "dummyValue")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(match, 400, 180)

# change frequency with an invalid value (negative)
netedit.modifyAdditionalDefaultValue(3, "-30")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(match, 400, 180)

# change frequency with a valid value
netedit.modifyAdditionalDefaultValue(3, "250")

# create calibrator with a valid parameter in other lane
netedit.leftClick(match, 400, 180)

# change routeprobe with a invalid  routeProbe
netedit.modifyAdditionalDefaultValue(4, "%%$···$%")

# create calibrator with a different routeProbe in other lane
netedit.leftClick(match, 200, 230)

# change frequency with a different routeProbe (Valid, empty)
netedit.modifyAdditionalDefaultValue(4, "")

# create calibrator with a valid parameter in other lane
netedit.leftClick(match, 200, 230)

# change routeprobe with a different routeProbe
netedit.modifyAdditionalDefaultValue(4, "routeProbe")

# create calibrator with a valid parameter in other lane
netedit.leftClick(match, 200, 230)

# change output with an invalid value 
netedit.modifyAdditionalDefaultValue(5, "%%%%###!!!")

# create calibrator with a valid parameter in other lane
netedit.leftClick(match, 200, 230)

# change output with an valid value 
netedit.modifyAdditionalDefaultValue(5, "myOwnOutput")

# create calibrator with a valid parameter in other lane
netedit.leftClick(match, 200, 230)

# Check undo redo
netedit.undo(match, 6)
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
