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

# @file    save.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
from ..constants import TEXTTEST_SANDBOX
from ..enums.attributesEnum import attrs
from ..enums.viewPositions import positions
from ..input.keyboard import typeKey, typeTwoKeys, typeThreeKeys, updateText
from ..input.mouse import moveMouse


def saveNewFile(referencePosition, element: str, extension: str, extensionIndex: int):
    """
    @brief save new file
    """
    filename = ""
    if (element == "network"):
        typeTwoKeys('ctrl', 's')
        filename = "net2"
    elif (element == "trafficLights"):
        typeThreeKeys('ctrl', 'shift', 'k')
        filename = "trafficlights2"
    elif (element == "edgeTypes"):
        typeThreeKeys('ctrl', 'shift', 'h')
        filename = "edgetypes2"
    elif (element == "additional"):
        typeThreeKeys('ctrl', 'shift', 'a')
        filename = "additionals2"
    elif (element == "demand"):
        typeThreeKeys('ctrl', 'shift', 'd')
        filename = "routes2"
    elif (element == "data"):
        typeThreeKeys('ctrl', 'shift', 'b')
        filename = "datas2"
    elif (element == "meanData"):
        typeThreeKeys('ctrl', 'shift', 'm')
        filename = "meandatas2.dat"
    elif (element == "xml"):
        typeTwoKeys('ctrl', 'l')
        filename = "net2"
    elif (element == "sumoConfig"):
        typeThreeKeys('ctrl', 'shift', 's')
        filename = "sumo2"
    elif (element == "neteditConfig"):
        typeThreeKeys('ctrl', 'shift', 'e')
        filename = "netedit2"
    elif (element == "joinedJunctions"):
        filename = "joinedjunctions2"
        # move mouse (to avoid problems with file menu)
        moveMouse(referencePosition, positions.reference, 200, 0, False)
        # go to menu command
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.saveJoinedJunctions):
            typeKey('down')
        typeKey('space')
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
    updateText(filename + "." + extension)
    typeKey('enter')
    # wait for load
    time.sleep(2)


def saveExistentFile(element):
    """
    @brief save existent element using shortcut (without opening a saving dialog)
    """
    if (element == "network"):
        typeTwoKeys('ctrl', 's')
    elif (element == "additionals"):
        typeThreeKeys('ctrl', 'shift', 'a')
    elif (element == "demands"):
        typeThreeKeys('ctrl', 'shift', 'd')
    elif (element == "datas"):
        typeThreeKeys('ctrl', 'shift', 'b')
    elif (element == "meanDatas"):
        typeThreeKeys('ctrl', 'shift', 'm')
    elif (element == "sumoConfig"):
        typeThreeKeys('ctrl', 'shift', 's')
    elif (element == "neteditConfig"):
        typeThreeKeys('ctrl', 'shift', 'e')
    # wait for debug (due recomputing)
    time.sleep(2)


def saveFileAs(referencePosition, type: str, multiple: bool):
    """
    @brief save the given element type as
    """
    # first obstain number of jumps and filename
    menuJumps = 0
    subMenuJumps = 0
    filename = ""
    extra = 0
    if (multiple):
        extra = 1
    if (type == "network"):
        menuJumps = attrs.toolbar.file.saveNetworkAs
        filename = "net3.net.xml"
    elif (type == "trafficLights"):
        menuJumps = attrs.toolbar.file.trafficLights.menu
        subMenuJumps = attrs.toolbar.file.trafficLights.saveAs + extra
        filename = "trafficlights3.tll.xml"
    elif (type == "edgeTypes"):
        menuJumps = attrs.toolbar.file.edgeTypes.menu
        subMenuJumps = attrs.toolbar.file.edgeTypes.saveAs + extra
        filename = "edgetypes3.typ.xml"
    elif (type == "additional"):
        menuJumps = attrs.toolbar.file.aditionalElements.menu
        subMenuJumps = attrs.toolbar.file.aditionalElements.saveAs + extra
        filename = "additionals3.add.xml"
    elif (type == "jupedsim"):
        menuJumps = attrs.toolbar.file.aditionalElements.menu
        subMenuJumps = attrs.toolbar.file.aditionalElements.saveJupedsim
        filename = "additionals3.add.xml"
    elif (type == "demand"):
        menuJumps = attrs.toolbar.file.demandElements.menu
        subMenuJumps = attrs.toolbar.file.demandElements.saveAs + extra
        filename = "routes3.rou.xml"
    elif (type == "data"):
        menuJumps = attrs.toolbar.file.dataElements.menu
        subMenuJumps = attrs.toolbar.file.dataElements.saveAs + extra
        filename = "datas3.dat.xml"
    elif (type == "meanData"):
        menuJumps = attrs.toolbar.file.meanDataElements.menu
        subMenuJumps = attrs.toolbar.file.meanDataElements.saveAs + extra
        filename = "meandatas3.dat.add.xml"
    elif (type == "sumoConfig"):
        menuJumps = attrs.toolbar.file.sumoConfig.menu
        subMenuJumps = attrs.toolbar.file.sumoConfig.saveAs
        filename = "sumo3.sumocfg"
    elif (type == "neteditConfig"):
        menuJumps = attrs.toolbar.file.neteditConfig.menu
        subMenuJumps = attrs.toolbar.file.neteditConfig.saveAs + extra
        filename = "netedit3.netecfg"
    # move mouse (to avoid problems with file menu)
    moveMouse(referencePosition, positions.reference, 200, 0, False)
    # go to menu command
    typeTwoKeys('alt', 'f')
    for _ in range(menuJumps):
        typeKey('down')
    typeKey('space')
    if (subMenuJumps > 0):
        for _ in range(subMenuJumps):
            typeKey('down')
        typeKey('space')
    # wait for dialog
    time.sleep(2)
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText(filename)
    typeKey('enter')
    # wait for load
    time.sleep(2)
