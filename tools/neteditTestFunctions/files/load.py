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
from ..constants import TEXTTEST_SANDBOX, DELAY_SAVING
from ..input.keyboard import typeKey, typeTwoKeys, typeThreeKeys, updateText
from ..input.mouse import leftClick


def openNeteditConfigAs(waitTime=2):
    """
    @brief load netedit config using dialog
    """
    # open save network as dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.netecfg")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def openSumoConfigAs(referencePosition):
    """
    @brief load netedit config using dialog
    """
    # click over reference (to avoid problem with undo-redo)
    leftClick(referencePosition, positions.reference)
    # open save network as dialog
    typeTwoKeys('ctrl', 'm')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.sumocfg")
    typeKey('enter')
    # wait for saving
    time.sleep(DELAY_SAVING)


def loadNetwork(useShortcut, waitTime=2):
    """
    @brief load network using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'o')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.loadNetwork):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("net.net.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadAdditionalElements(useShortcut, waitTime=2):
    """
    @brief load additional using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'a')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.aditionalElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.aditionalElements.load):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additionals.add.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadDemandElements(useShortcut, waitTime=2):
    """
    @brief load demand elements using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'd')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.demandElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.demandElements.load):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("routes.rou.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadDataElements(useShortcut, waitTime=2):
    """
    @brief load data elements using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'b')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.dataElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.dataElements.load):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datas.dat.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadMeanDataElements(waitTime=2):
    """
    @brief load mean data elements using dialog
    """
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
    # wait for saving
    time.sleep(waitTime)
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datas.med.add.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def openNeteditConfigShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("netedit_open.netecfg")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)


def openNetworkShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.net.xml")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)


def openConfigurationShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeThreeKeys('ctrl', 'shift', 'o')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.netccfg")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)
