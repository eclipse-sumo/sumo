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

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect calibrator
netedit.leftClick(referencePosition, 297, 116)

# Change generic parameters with an invalid value (dummy)
netedit.modifyAttribute(6, "dummyGenericParameters", True)

# Change generic parameters with an invalid value (invalid format)
netedit.modifyAttribute(6, "key1|key2|key3", True)

# Change generic parameters with a valid value
netedit.modifyAttribute(6, "key1=value1|key2=value2|key3=value3", True)

# Change generic parameters with a valid value (empty values)
netedit.modifyAttribute(6, "key1=|key2=|key3=", True)

# Change generic parameters with a valid value (clear parameters)
netedit.modifyAttribute(6, "", True)

# Change generic parameters with an valid value (duplicated keys)
netedit.modifyAttribute(6, "key1duplicated=value1|key1duplicated=value2|key3=value3", True)

# Change generic parameters with a valid value (duplicated values)
netedit.modifyAttribute(6, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", True)

# Change generic parameters with an invalid value (invalid key characters)
netedit.modifyAttribute(6, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3", True)

# Change generic parameters with a invalid value (invalid value characters)
netedit.modifyAttribute(6, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3", True)

# Change generic parameters with a valid value
netedit.modifyAttribute(6, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3", True)

# Check undo redo
netedit.undo(referencePosition, 9)
netedit.redo(referencePosition, 9)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)
