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

# @file    reload.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
import pyautogui
from ..constants import DELAY_RELOAD
from ..enums.attributesEnum import attrs
from ..enums.viewPositions import positions
from ..general.functions import waitQuestion
from ..input.keyboard import typeKey, typeTwoKeys
from ..input.mouse import moveMouse


def reload(NeteditProcess, openNetDialog=False, saveNet=False,
           openAdditionalDialog=False, saveAdditionalElements=False,
           openDemandDialog=False, saveDemandElements=False,
           openDataDialog=False, saveDataElements=False,
           openMeanDataDialog=False, saveMeanDataElements=False):
    """
    @brief reload Netedit
    """
    # first move cursor out of magenta square
    pyautogui.moveTo(150, 200)
    # reload using hotkey
    typeTwoKeys('ctrl', 'r')
    # Check if net must be saved
    if openNetDialog:
        if saveNet:
            waitQuestion('s')
        else:
            waitQuestion('n')
    # Check if additionals must be saved
    if openAdditionalDialog:
        if saveAdditionalElements:
            waitQuestion('s')
        else:
            waitQuestion('n')
    # Check if demand elements must be saved
    if openDemandDialog:
        if saveDemandElements:
            waitQuestion('s')
        else:
            waitQuestion('n')
    # Check if data elements must be saved
    if openDataDialog:
        if saveDataElements:
            waitQuestion('s')
        else:
            waitQuestion('n')
    # Check if meanData elements must be saved
    if openMeanDataDialog:
        if saveMeanDataElements:
            waitQuestion('s')
        else:
            waitQuestion('n')
    # Wait some seconds
    time.sleep(DELAY_RELOAD)
    # check if Netedit was crashed during reloading
    if NeteditProcess.poll() is not None:
        print("TestFunctions: Error reloading Netedit")


def reloadFile(referencePosition, type: str, multiple: bool):
    """
    @brief reload file
    """
    extra = 0
    if (multiple):
        extra = 1
    # move mouse (to avoid problems with file menu)
    moveMouse(referencePosition, positions.reference, 200, 0, False)
    # open load mean data dialog (because doesn't have shortcut)
    typeTwoKeys('alt', 'f')
    # continue depending of type
    if (type == "network"):
        for _ in range(attrs.toolbar.file.reloadNetwork):
            typeKey('down')
        typeKey('space')
    elif (type == "sumoConfig"):
        for _ in range(attrs.toolbar.file.sumoConfig.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.sumoConfig.reload):
            typeKey('down')
        typeKey('space')
    elif (type == "neteditConfig"):
        for _ in range(attrs.toolbar.file.neteditConfig.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.neteditConfig.reload):
            typeKey('down')
        typeKey('space')
    elif (type == "trafficLights"):
        for _ in range(attrs.toolbar.file.trafficLights.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.trafficLights.reload):
            typeKey('down')
        typeKey('space')
    elif (type == "edgeTypes"):
        for _ in range(attrs.toolbar.file.edgeTypes.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.edgeTypes.reload):
            typeKey('down')
        typeKey('space')
    elif (type == "additional"):
        for _ in range(attrs.toolbar.file.aditionalElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.aditionalElements.reload + extra):
            typeKey('down')
        typeKey('space')
    elif (type == "demand"):
        for _ in range(attrs.toolbar.file.demandElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.demandElements.reload + extra):
            typeKey('down')
        typeKey('space')
    elif (type == "data"):
        for _ in range(attrs.toolbar.file.dataElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.dataElements.reload + extra):
            typeKey('down')
        typeKey('space')
    elif (type == "meanData"):
        for _ in range(attrs.toolbar.file.meanDataElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.meanDataElements.reload + extra):
            typeKey('down')
        typeKey('space')
    time.sleep(2)
