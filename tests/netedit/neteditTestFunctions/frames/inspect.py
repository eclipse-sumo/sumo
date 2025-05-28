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

# @file    inspect.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ..general.functions import *

def modifyAttribute(attributeIndex, value, overlapped):
    """
    @brief modify attribute of type int/float/string
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    # paste the new value
    updateText(value)
    # type Enter to commit change
    typeKey('enter')


def modifyBoolAttribute(attributeIndex, overlapped):
    """
    @brief modify boolean attribute
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    # type SPACE to change value
    typeKey('space')


def modifyColorAttribute(attributeIndex, color, overlapped):
    """
    @brief modify color using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    typeKey('space')
    # go to list of colors TextField
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    # select color
    for _ in range(1 + color):
        typeKey('down')
    # go to accept button and press it
    typeKey('tab')
    typeKey('space')


def modifyAttributeVClassDialog(attribute, vClass, overlapped, disallowAll=True, cancel=False, reset=False):
    """
    @brief modify vclass attribute using dialog
    """
    # open dialog
    modifyBoolAttribute(attribute, overlapped)
    # first check if disallow all
    if (disallowAll):
        for _ in range(attrs.dialog.allowVClass.disallowAll):
            typeKey('tab')
        typeKey('space')
        # go to vClass
        for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
            typeKey('tab')
        # Change current value
        typeKey('space')
    else:
        # go to vClass
        for _ in range(vClass):
            typeKey('tab')
        # Change current value
        typeKey('space')
    # check if cancel
    if (cancel):
        for _ in range(attrs.dialog.allowVClass.cancel - vClass):
            typeKey('tab')
        typeKey('space')
    elif (reset):
        for _ in range(attrs.dialog.allowVClass.reset - vClass):
            typeKey('tab')
        typeKey('space')
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    else:
        for _ in range(attrs.dialog.allowVClass.accept - vClass):
            typeKey('tab')
        typeKey('space')


def modifyAdditionalFileDialog(attributeIndex, overlapped, waitTime=2):
    """
    @brief modify default additional file using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    # Change current value
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additional.secondFile.add.xml")
    typeKey('enter')


def modifyAdditionalFile(attributeIndex, overlapped):
    """
    @brief modify default additional file
    """
    modifyAttribute(attributeIndex, TEXTTEST_SANDBOX + "/additional.thirdFile.add.xml", overlapped)


def checkUndoRedo(referencePosition, offsetX=0, offsetY=0):
    """
    @brief Check undo-redo
    """
    # Check undo
    undo(referencePosition, 9, offsetX)
    # Check redo
    redo(referencePosition, 9, offsetY)


def checkParameters(referencePosition, attributeIndex, overlapped, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeIndex, "dummyGenericParameters", overlapped)
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeIndex, "key1|key2|key3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "key1=value1|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeIndex, "key1=|key2=|key3=", overlapped)
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeIndex, "", overlapped)
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeIndex, "key1duplicated=value1|key1duplicated=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", overlapped)
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeIndex, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3", overlapped)
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)


def checkDoubleParameters(referencePosition, attributeIndex, overlapped, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeIndex, "dummyGenericParameters", overlapped)
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeIndex, "key1|key2|key3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "key1=1|key2=2|key3=3", overlapped)
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeIndex, "key1=|key2=|key3=", overlapped)
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeIndex, "", overlapped)
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeIndex, "key1duplicated=1|key1duplicated=2|key3=3", overlapped)
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", overlapped)
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeIndex, "keyInvalid.;%>%$$=1|key2=2|key3=3", overlapped)
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=2|key3=3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "keyFinal1=1|keyFinal2=2|keyFinal3=3", overlapped)
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)