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

# select busStop
netedit.changeAdditional("busStop")

# change reference to center
netedit.modifyAdditionalDefaultValue(7, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(referencePosition, 250, 170)

# go to inspect mode
netedit.inspectMode()

# inspect first busStop
netedit.leftClick(referencePosition, 250, 190)

# Change generic parameters with a dummy value
netedit.modifyAttribute(9, "dummyGenericParameters", True)

# Change generic parameters with a invalid format
netedit.modifyAttribute(9, "key1|key2|key3", True)

# Change generic parameters with a valid value
netedit.modifyAttribute(9, "key1=value1|key2=value2|key3=value3", True)

# Change generic parameters with a valid value (empty values)
netedit.modifyAttribute(9, "key1=|key2=|key3=", True)

# Change generic parameters with a valid value (all empty)
netedit.modifyAttribute(9, "", True)

# Change generic parameters with an invalid value (duplicated)
netedit.modifyAttribute(9, "key1duplicated=value1|key1duplicated=value2|key3=value3", True)

# Change generic parameters with a valid value
netedit.modifyAttribute(9, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", True)

# Change generic parameters with an invalid value (invalid key characters)
netedit.modifyAttribute(9, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3", True)

# Change generic parameters with a invalid value (invalid value characters)
netedit.modifyAttribute(9, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3", True)

# Change generic parameters with a valid value
netedit.modifyAttribute(9, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3", True)

# Check undo redo
netedit.undo(referencePosition, 8)
netedit.redo(referencePosition, 8)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
