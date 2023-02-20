#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2019-07-16

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

# go to demand mode
netedit.supermodeDemand()

# go to container mode
netedit.containerMode()

# change Container
netedit.changeElement("containerFlow")

# change container plan
netedit.changeContainerPlan("tranship: edge->containerStop", True)

# set invalid arrival pos
netedit.changeDefaultValue(netedit.attrs.containerFlow.create.terminate, "dummyTerminate")

# create route using edge and containerStop
netedit.leftClick(referencePosition, 274, 400)
netedit.leftClick(referencePosition, 270, 43)

# press enter to create route
netedit.typeEnter()

# set invalid arrival pos
netedit.changeDefaultValue(netedit.attrs.containerFlow.create.terminate, "number")

# press enter to create route
netedit.typeEnter()

# create route using edge and containerStop
netedit.leftClick(referencePosition, 274, 400)
netedit.leftClick(referencePosition, 270, 43)

# set valid arrival pos
netedit.changeDefaultValue(netedit.attrs.containerFlow.create.terminateOption, "dummy")

# press enter to create route
netedit.typeEnter()

# set valid arrival pos
netedit.changeDefaultValue(netedit.attrs.containerFlow.create.terminateOption, "-30")

# press enter to create route
netedit.typeEnter()

# set valid arrival pos
netedit.changeDefaultValue(netedit.attrs.containerFlow.create.terminateOption, "20.5")

# press enter to create route
netedit.typeEnter()

# set valid arrival pos
netedit.changeDefaultValue(netedit.attrs.containerFlow.create.terminateOption, "22")

# press enter to create route
netedit.typeEnter()

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save Netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
