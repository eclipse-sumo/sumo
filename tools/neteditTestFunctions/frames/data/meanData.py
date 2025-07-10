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

# @file    meanData.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
from ...general.functions import focusOnFrame
from ...input.keyboard import typeKey


def createMeanData():
    """
    @brief create mean data
    """
    # focus current frame
    focusOnFrame()
    # go to create mean data
    for _ in range(5):
        typeKey('tab')
    # create mean data
    typeKey('space')


def deleteMeanData():
    """
    @brief delete mean data
    """
    # focus current frame
    focusOnFrame()
    # go to delete mean data
    for _ in range(6):
        typeKey('tab')
    # delete mean data
    typeKey('space')


def copyMeanData():
    """
    @brief copy mean data
    """
    # focus current frame
    focusOnFrame()
    # go to copy mean data
    for _ in range(7):
        typeKey('tab')
    # copy mean data
    typeKey('space')
