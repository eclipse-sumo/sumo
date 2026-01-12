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

# @file    crossing.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...enums.attributesEnum import attrs
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey, updateText


def createCrossing(hasTLS):
    """
    @brief create crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to create crossing button depending of hasTLS
    if hasTLS:
        for _ in range(attrs.crossing.createTLS.button):
            typeKey('tab')
    else:
        for _ in range(attrs.crossing.create.button):
            typeKey('tab')
    # type space to create crossing
    typeKey('space')


def modifyCrossingDefaultValue(numtabs, value):
    """
    @brief change default int/real/string crossing default value
    """
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + attrs.crossing.firstField):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def modifyCrossingDefaultBoolValue(numtabs):
    """
    @brief change default boolean crossing default value
    """
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + attrs.crossing.firstField):
        typeKey('tab')
    # type space to change value
    typeKey('space')


def crossingClearEdges():
    """
    @brief clear crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to clear button
    for _ in range(attrs.crossing.clearEdges):
        typeKey('tab')
    # type space to activate button
    typeKey('space')


def crossingInvertEdges():
    """
    @brief invert crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to invert button
    for _ in range(attrs.crossing.invertEdges):
        typeKey('tab')
    # type space to activate button
    typeKey('space')
