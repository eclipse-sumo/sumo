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
# @date    2016-11-25

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

# select BusStop
netedit.changeElement("busStop")

# create BusStop with default parameters
netedit.leftClick(referencePosition, 428, 257)

# select Access
netedit.changeElement("access")

# Create Access
netedit.selectAdditionalChild(netedit.attrs.access.create.parent, 0)
netedit.leftClick(referencePosition, 153, 95)

# go to inspect mode
netedit.inspectMode()

# delete Access
netedit.leftClick(referencePosition, 153, 95)

# Change parameter pos with a non valid value (dummy position X)
netedit.modifyAttribute(netedit.attrs.access.inspect.pos, "dummy position", True)

# Change parameter pos with a valid value (empty)
netedit.modifyAttribute(netedit.attrs.access.inspect.pos, "", True)

# Change parameter pos with a valid value (negativ)
netedit.modifyAttribute(netedit.attrs.access.inspect.pos, "-1000", True)

# Change parameter pos with a valid value (greather than lane length)
netedit.modifyAttribute(netedit.attrs.access.inspect.pos, "1000", True)

# Change parameter pos with a valid value (middle lane)
netedit.modifyAttribute(netedit.attrs.access.inspect.pos, "2.1", True)

# Check undo redo
netedit.undo(referencePosition, 6)
netedit.redo(referencePosition, 6)

# save netedit config
netedit.saveNeteditConfig(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
