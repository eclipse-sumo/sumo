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

# recompute
netedit.rebuildNetwork()

# toogle select lanes
netedit.changeEditMode('2')

# go to select mode
netedit.selectMode()

# select first edge
netedit.leftClick(referencePosition, 250, 180)

# select second edge
netedit.leftClick(referencePosition, 250, 100)

# go to inspect mode
netedit.inspectMode()

# inspect edge
netedit.leftClick(referencePosition, 250, 180)

# Change generic parameters with an invalid value (dummy)
netedit.modifyAttribute(9, "dummyGenericParameters", False)

# Change generic parameters with an invalid value (invalid format)
netedit.modifyAttribute(9, "key1|key2|key3", False)

# Change generic parameters with a valid value
netedit.modifyAttribute(9, "key1=value1|key2=value2|key3=value3", False)

# Change generic parameters with a valid value (empty values)
netedit.modifyAttribute(9, "key1=|key2=|key3=", False)

# Change generic parameters with a valid value (clear parameters)
netedit.modifyAttribute(9, "", False)

# Change generic parameters with an valid value (duplicated keys)
netedit.modifyAttribute(9, "key1duplicated=value1|key1duplicated=value2|key3=value3", False)

# Change generic parameters with a valid value (duplicated values)
netedit.modifyAttribute(9, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", False)

# Change generic parameters with an invalid value (invalid key characters)
netedit.modifyAttribute(9, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3", False)

# Change generic parameters with a invalid value (invalid value characters)
netedit.modifyAttribute(9, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3", False)

# Change generic parameters with a valid value
netedit.modifyAttribute(9, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3", False)

# recompute
netedit.rebuildNetwork()

# Check undos
netedit.undo(referencePosition, 7)

# recompute
netedit.rebuildNetwork()

# check redos
netedit.redo(referencePosition, 7)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
