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
# @date    2016-11-25

# import common functions for netedit tests
import os
import sys

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart()

# go to TLS mode
netedit.changeMode("TLS")

# select junction
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.center)

# create TLS
netedit.createTLS()

# join TLS
netedit.joinTSL()

# select four corners
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.up)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.down)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.left)
netedit.leftClick(referencePosition, netedit.positions.network.junction.cross.right)

# join tls
netedit.typeKey("enter")

# Change generic parameters with an invalid value (dummy)
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoined.parameters, "dummyGenericParameters")

# Change generic parameters with an invalid value (invalid format)
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoined.parameters, "key1|key2|key3")

# Change generic parameters with a valid value
netedit.modifyAttribute(netedit.attrs.TLS.attributesJoined.parameters,
                        "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3")

# type enter to save changes
netedit.typeKey("enter")

# type ESC (for undo-redo)
netedit.typeKey("esc")

# Check undo
netedit.undo(referencePosition, 1)

# Check redo
netedit.redo(referencePosition, 1)

# save Netedit config
netedit.saveExistentShortcut("neteditConfig")

# quit netedit
netedit.quit(neteditProcess)
