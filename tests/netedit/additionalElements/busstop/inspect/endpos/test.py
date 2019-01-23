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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeAdditional("busStop")

# change reference to center
netedit.modifyAdditionalDefaultValue(6, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(referencePosition, 250, 170)

# go to inspect mode
netedit.inspectMode()

# inspect first busStop
netedit.leftClick(referencePosition, 250, 190)

# Change parameter endpos with a valid value (empty)
netedit.modifyAttribute(7, "")

# Change parameter endpos with a valid value (> lanelenght)
netedit.modifyAttribute(7, "5000")

# Change parameter endpos with a non valid value (< startPos)
netedit.modifyAttribute(7, "2")

# Change parameter endpos with a valid value
netedit.modifyAttribute(7, "30")

# Check undos and redos
netedit.undo(referencePosition, 4)
netedit.redo(referencePosition, 4)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
