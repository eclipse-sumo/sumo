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

# @file    common.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ..general.functions import *


def changeElement(frame, element):
    """
    @brief change element in the given frame (Additional, shape, vehicle...)
    """
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    if (frame == "additionalFrame"):
        for _ in range(attrs.frames.changeElement.additional):
            typeKey('tab')
    elif (frame == "shapeFrame"):
        for _ in range(attrs.frames.changeElement.shape):
            typeKey('tab')
    elif (frame == "vehicleFrame"):
        for _ in range(attrs.frames.changeElement.vehicle):
            typeKey('tab')
    elif (frame == "personFrame"):
        for _ in range(attrs.frames.changeElement.person):
            typeKey('tab')
    elif (frame == "containerFrame"):
        for _ in range(attrs.frames.changeElement.container):
            typeKey('tab')
    # paste the new value
    updateText(element)
    # type enter to save change
    typeKey('enter')


def changeDefaultValue(attributeIndex, value):
    """
    @brief modify default int/double/string value of an additional, shape, vehicle...
    """
    # focus current frame
    focusOnFrame()
    # go to value TextField
    for _ in range(attributeIndex):
        typeKey('tab')
    # paste new value
    updateText(value)
    # type enter to save new value
    typeKey('enter')


def changeDefaultBoolValue(attributeIndex):
    """
    @brief modify default bool value of an additional, shape, vehicle...
    """
    # focus current frame
    focusOnFrame()
    # place cursor in check Box position
    for _ in range(attributeIndex):
        typeKey('tab')
    # Change current value
    typeKey('space')


def changeDefaultAllowDisallowValue(attributeIndex):
    """
    @brief modify allow/disallow values
    """
    # open dialog
    changeDefaultBoolValue(attributeIndex)
    # select vtypes
    for _ in range(2):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # select vtypes
    for _ in range(6):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # select vtypes
    for _ in range(12):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # select vtypes
    for _ in range(11):
        typeKey('tab')
    # Change current value
    typeKey('space')
