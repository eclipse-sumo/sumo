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

# @file    colorAttribute.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...enums.attributesEnum import attrs
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey, typeTwoKeys


def modifyColorAttribute(attributeIndex):
    """
    @brief modify color using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for _ in range(attributeIndex):
        typeKey('tab')
    typeKey('space')
    # go to list of colors TextField
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    # select color
    for _ in range(6):
        typeKey('down')
    # go to accept button and press it
    typeKey('tab')
    typeKey('space')


def modifyColorAttributeOverlapped(attributeIndex):
    """
    @brief modify color using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for _ in range(attributeIndex + attrs.editElements.overlapped):
        typeKey('tab')
    typeKey('space')
    # go to list of colors TextField
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    # select color
    for _ in range(6):
        typeKey('down')
    # go to accept button and press it
    typeKey('tab')
    typeKey('space')
