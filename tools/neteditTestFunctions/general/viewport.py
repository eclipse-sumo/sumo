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

# @file    viewport.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..constants import TEXTTEST_SANDBOX, DELAY_SELECT
from ..input.keyboard import typeKey, typeTwoKeys, updateText


def loadViewPort():
    """
    @brief load viewport
    """
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # go to load
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("loadViewport.xml")
    typeKey('enter')
    # wait
    time.sleep(DELAY_SELECT)
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


def saveViewPort():
    """
    @brief save viewport
    """
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # go to save
    typeKey('tab')
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("viewport.xml")
    typeKey('enter')
    # wait
    time.sleep(DELAY_SELECT)
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


def setViewport(zoom, x, y, z, r):
    """
    @brief edit viewport
    """
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # go to zoom
    for _ in range(2):
        typeKey('tab')
    # Paste X
    if (len(zoom) > 0):
        updateText(zoom)
    # go to Y
    typeKey('tab')
    # Paste X
    if (len(x) > 0):
        updateText(x)
    # go to Y
    typeKey('tab')
    # Paste Y
    if (len(y) > 0):
        updateText(y)
    # go to Z
    typeKey('tab')
    # Paste Z
    if (len(z) > 0):
        updateText(z)
    # go to rotation
    typeKey('tab')
    # Paste rotation
    if (len(r) > 0):
        updateText(r)
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')
