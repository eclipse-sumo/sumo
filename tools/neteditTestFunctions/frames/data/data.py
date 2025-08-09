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

# @file    data.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey, updateText


def createDataSet(dataSetID):
    """
    @brief create dataSet
    """
    # focus current frame
    focusOnFrame()
    # go to create new dataSet
    for _ in range(2):
        typeKey('tab')
    # enable create dataSet
    typeKey('space')
    # create new ID
    updateText(dataSetID)
    # go to create new dataSet button
    typeKey('tab')
    # create dataSet
    typeKey('space')


def createDataInterval(begin, end):
    """
    @brief create dataInterval
    """
    # focus current frame
    focusOnFrame()
    # go to create new dataInterval
    for _ in range(5):
        typeKey('tab')
    typeKey('tab')
    # enable create dataInterval
    typeKey('space')
    # go to create new dataInterval begin
    typeKey('tab')
    # set begin
    updateText(begin)
    # go to end
    typeKey('tab')
    # set end
    updateText(end)
    # go to create new dataSet button
    typeKey('tab')
    # create dataSet
    typeKey('space')
