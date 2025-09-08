#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# go to demand mode
netedit.changeSupermode("demand")

# go to person mode
netedit.changeMode("person")

# change Person
netedit.changeElement("personFrame", "personFlow")

# create person using three edges
netedit.leftClick(referencePosition, netedit.positions.elements.edge0)
netedit.leftClick(referencePosition, netedit.positions.elements.edge1)

# press enter to create person
netedit.typeKey("enter")

# create person using three edges
netedit.leftClick(referencePosition, netedit.positions.elements.edge1)
netedit.leftClick(referencePosition, netedit.positions.elements.edge2)

# press enter to create person
netedit.typeKey("enter")

# go to inspect mode
netedit.changeMode("inspect")

# inspect person
netedit.leftClick(referencePosition, netedit.positions.elements.demands.person)

# change ID with an invalid value
netedit.modifyAttribute(netedit.attrs.personFlow.inspect.id, "")

# change ID with an invalid value
netedit.modifyAttribute(netedit.attrs.personFlow.inspect.id, ";;;;;;;;")

# change ID with an invalid value
netedit.modifyAttribute(netedit.attrs.personFlow.inspect.id, "id with spaces")

# change ID with an invalid value
netedit.modifyAttribute(netedit.attrs.personFlow.inspect.id, "pf_1")

# change ID with an invalid value (empty)
netedit.modifyAttribute(netedit.attrs.personFlow.inspect.id, "customID")

# Check undo
netedit.checkUndoRedo(referencePosition)

# save Netedit config
netedit.saveExistentFile("neteditConfig")

# save persons
# quit netedit
netedit.quit(neteditProcess)
