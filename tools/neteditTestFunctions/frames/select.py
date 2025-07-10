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

# @file    select.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import os
import pyautogui
import time
from ..enums.attributesEnum import attrs
from ..constants import TEXTTEST_SANDBOX, DELAY_SELECT, DELAY_KEY
from ..general.functions import focusOnFrame
from ..input.mouse import dragDrop
from ..input.keyboard import keyPress, keyRelease, typeKey, typeTwoKeys, updateText


def selection(selectionType):
    """
    @brief do a selection
    """
    # focus current frame
    focusOnFrame()
    if (selectionType == "default"):
        for _ in range(attrs.frames.selection.basic.default):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "save"):
        # jump to save
        for _ in range(attrs.frames.selection.basic.save):
            typeKey('tab')
        typeKey('space')
        # jump to filename TextField
        typeTwoKeys('alt', 'f')
        filename = os.path.join(TEXTTEST_SANDBOX, "selection.txt")
        updateText(filename)
        # type enter to select it
        typeKey('enter')
    elif (selectionType == "load"):
        # jump to save
        for _ in range(attrs.frames.selection.basic.load):
            typeKey('tab')
        typeKey('space')
        # jump to filename TextField
        typeTwoKeys('alt', 'f')
        filename = os.path.join(TEXTTEST_SANDBOX, "selection.txt")
        updateText(filename)
        # type enter to select it
        typeKey('enter')
    elif (selectionType == "add"):
        # jump to mode "add"
        for _ in range(attrs.frames.selection.basic.add):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "remove"):
        # jump to mode "remove"
        for _ in range(attrs.frames.selection.basic.remove):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "keep"):
        # jump to mode "keep"
        for _ in range(attrs.frames.selection.basic.keep):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "replace"):
        # jump to mode "replace"
        for _ in range(attrs.frames.selection.basic.replace):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "clear"):
        for _ in range(attrs.frames.selection.basic.clear):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "invert"):
        for _ in range(attrs.frames.selection.basic.invert):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "invertData"):
        for _ in range(attrs.frames.selection.basic.invertData):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    elif (selectionType == "delete"):
        for _ in range(attrs.frames.selection.basic.delete):
            typeKey('tab')
        # type space to select it
        typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def lockSelection(glType):
    """
    @brief lock selection by glType
    """
    # focus current frame
    focusOnFrame()
    # move mouse
    pyautogui.moveTo(550, 200)
    # open Lock menu
    typeTwoKeys('alt', 'o')
    # go to selected glType
    for _ in range(glType):
        typeKey("down")
    # type enter to save change
    typeKey('space')


def selectNetworkItems(element, attribute, value):
    """
    @brief select network items
    """
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for _ in range(attrs.frames.selection.networkItem.type):
        typeKey('tab')
    # paste the new elementClass
    updateText("Network elements")
    # jump to element
    for _ in range(attrs.frames.selection.networkItem.subType):
        typeKey('tab')
    # paste the new elementType
    updateText(element)
    # jump to attribute
    for _ in range(attrs.frames.selection.networkItem.attribute):
        typeKey('tab')
    # paste the new attribute
    updateText(attribute)
    # jump to value
    for _ in range(attrs.frames.selection.networkItem.value):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to select it
    typeKey('enter')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectStoppingPlaceItems(elementClass, stoppingPlace, elementType, attribute, value):
    """
    @brief select items
    """
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for _ in range(8):
        typeKey('tab')
    # paste the new elementClass
    updateText(elementClass)
    # jump to element
    for _ in range(2):
        typeKey('tab')
    # paste the new elementType
    updateText(stoppingPlace)
    # jump to stoppingPlace
    for _ in range(2):
        typeKey('tab')
    # paste the new elementType
    updateText(elementType)
    # jump to attribute
    for _ in range(3):
        typeKey('tab')
    # paste the new attribute
    updateText(attribute)
    # jump to value
    for _ in range(2):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to select it
    typeKey('enter')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionRectangle(referencePosition, positionA, positionB):
    """
    @brief select using an rectangle
    """
    # Leave Shift key pressedX
    keyPress('shift')
    # move element
    dragDrop(referencePosition, positionA.x, positionA.y, positionB.x, positionB.y)
    # wait after key up
    time.sleep(DELAY_KEY)
    # Release Shift key
    keyRelease('shift')
    # wait for gl debug
    time.sleep(DELAY_SELECT)
