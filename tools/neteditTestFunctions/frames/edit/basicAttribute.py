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

# @file    basicAttribute.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...enums.attributesEnum import attrs
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey, updateText


def modifyAttribute(attributeIndex, value):
    """
    @brief modify attribute of type int/float/string
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for _ in range(attributeIndex):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type Enter to commit change
    typeKey('enter')


def modifyAttributeOverlapped(attributeIndex, value):
    """
    @brief modify attribute of type int/float/string
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for _ in range(attributeIndex + attrs.editElements.overlapped):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type Enter to commit change
    typeKey('enter')
