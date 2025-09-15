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

# @file    load.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..enums.attributesEnum import attrs
from ..enums.viewPositions import positions
from ..constants import TEXTTEST_SANDBOX
from ..input.keyboard import typeKey, typeTwoKeys, typeThreeKeys, updateText
from ..input.mouse import moveMouse


def loadFile(referencePosition, type: str, file: str, extension: str, extensionIndex: int):
    """
    @brief load file config using dialog
    """
    # continue depending of type
    if (type == "neteditConfig"):
        typeTwoKeys('ctrl', 'e')
    elif (type == "sumoConfig"):
        typeTwoKeys('ctrl', 'm')
    elif (type == "netconvertConfig"):
        typeThreeKeys('ctrl', 'shift', 'o')
    elif (type == "network"):
        typeTwoKeys('ctrl', 'o')
    elif (type == "trafficLights"):
        typeTwoKeys('ctrl', 'k')
    elif (type == "edgeTypes"):
        typeTwoKeys('ctrl', 'h')
    elif (type == "additional"):
        typeTwoKeys('ctrl', 'a')
    elif (type == "demand"):
        typeTwoKeys('ctrl', 'd')
    elif (type == "data"):
        typeTwoKeys('ctrl', 'b')
    elif (type == "meanData"):
        # move mouse (to avoid problems with file menu)
        moveMouse(referencePosition, positions.reference, 200, 0, False)
        # open load mean data dialog (because doesn't have shortcut)
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.meanDataElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.meanDataElements.load):
            typeKey('down')
        typeKey('space')
        # jump to filename TextField
        typeTwoKeys('alt', 'f')
    # wait for dialog
    time.sleep(2)
    # set folder
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    # set extension
    typeKey('tab')
    for _ in range(0, extensionIndex):
        typeKey('down')
    typeTwoKeys('shift', 'tab')
    # set file
    updateText(file + "." + extension)
    typeKey('enter')
    # wait for load
    time.sleep(2)
