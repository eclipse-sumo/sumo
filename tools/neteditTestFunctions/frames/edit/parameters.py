# -*- coding: utf-8 -*-
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

# @file    parameters.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...frames.edit.basicAttribute import modifyAttribute, modifyAttributeOverlapped
from ...general.undoRedo import checkUndoRedo


def checkParameters(referencePosition, attributeIndex, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeIndex, "dummyGenericParameters")
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeIndex, "key1|key2|key3")
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "key1=value1|key2=value2|key3=value3")
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeIndex, "key1=|key2=|key3=")
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeIndex, "")
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeIndex, "key1duplicated=value1|key1duplicated=value2|key3=value3")
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated")
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeIndex, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3")
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3")
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3")
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)


def checkParametersOverlapped(referencePosition, attributeIndex, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttributeOverlapped(attributeIndex, "dummyGenericParameters")
    # Change generic parameters with an invalid value (invalid format)
    modifyAttributeOverlapped(attributeIndex, "key1|key2|key3")
    # Change generic parameters with a valid value
    modifyAttributeOverlapped(attributeIndex, "key1=value1|key2=value2|key3=value3")
    # Change generic parameters with a valid value (empty values)
    modifyAttributeOverlapped(attributeIndex, "key1=|key2=|key3=")
    # Change generic parameters with a valid value (clear parameters)
    modifyAttributeOverlapped(attributeIndex, "")
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttributeOverlapped(attributeIndex, "key1duplicated=value1|key1duplicated=value2|key3=value3")
    # Change generic parameters with a valid value (duplicated values)
    modifyAttributeOverlapped(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated")
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttributeOverlapped(attributeIndex, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3")
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttributeOverlapped(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3")
    # Change generic parameters with a valid value
    modifyAttributeOverlapped(attributeIndex, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3")
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)


def checkDoubleParameters(referencePosition, attributeIndex, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters (doubles)
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeIndex, "dummyGenericParameters")
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeIndex, "key1|key2|key3")
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "key1=1|key2=2|key3=3")
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeIndex, "key1=|key2=|key3=")
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeIndex, "")
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeIndex, "key1duplicated=1|key1duplicated=2|key3=3")
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated")
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeIndex, "keyInvalid.;%>%$$=1|key2=2|key3=3")
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=2|key3=3")
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "keyFinal1=1|keyFinal2=2|keyFinal3=3")
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY + 30)


def checkDoubleParametersOverlapped(referencePosition, attributeIndex, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters (doubles)
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttributeOverlapped(attributeIndex, "dummyGenericParameters")
    # Change generic parameters with an invalid value (invalid format)
    modifyAttributeOverlapped(attributeIndex, "key1|key2|key3")
    # Change generic parameters with a valid value
    modifyAttributeOverlapped(attributeIndex, "key1=1|key2=2|key3=3")
    # Change generic parameters with a valid value (empty values)
    modifyAttributeOverlapped(attributeIndex, "key1=|key2=|key3=")
    # Change generic parameters with a valid value (clear parameters)
    modifyAttributeOverlapped(attributeIndex, "")
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttributeOverlapped(attributeIndex, "key1duplicated=1|key1duplicated=2|key3=3")
    # Change generic parameters with a valid value (duplicated values)
    modifyAttributeOverlapped(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated")
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttributeOverlapped(attributeIndex, "keyInvalid.;%>%$$=1|key2=2|key3=3")
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttributeOverlapped(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=2|key3=3")
    # Change generic parameters with a valid value
    modifyAttributeOverlapped(attributeIndex, "keyFinal1=1|keyFinal2=2|keyFinal3=3")
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY + 30)
