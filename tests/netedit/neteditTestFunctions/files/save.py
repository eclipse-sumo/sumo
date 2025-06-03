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
from ..imports import *
from ..constants import *
from ..input.keyboard import *
from ..input.mouse import *


def saveNeteditConfigNew(waitTime=2):
    """
    @brief save netedit config after opening a new network
    """
    # save netedit config using hotkey
    typeThreeKeys('ctrl', 'shift', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    typeKey('enter')
    updateText("configAs.netecfg")
    typeKey('enter')
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def saveNeteditConfigAs(referencePosition, waitTime=2):
    """
    @brief save configuration as using shortcut
    """
    # move cursor
    leftClick(referencePosition, 500, 0)
    # go to save netedit config
    typeTwoKeys('alt', 'f')
    for _ in range(14):
        typeKey('down')
    typeKey('right')
    typeKey('down')
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("saveConfigAs.netecfg")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)


def savePlainXML(waitTime=2):
    """
    @brief save configuration using shortcut
    """
    # open configuration dialog
    typeTwoKeys('ctrl', 'l')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("net")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)


def saveNeteditConfig(referencePosition, clickOverReference=False):
    """
    @brief save netedit config
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, 0, 0)
    # save netedit config using hotkey
    typeThreeKeys('ctrl', 'shift', 'e')
    # wait for saving
    time.sleep(DELAY_SAVING)


def saveSumoConfig(referencePosition):
    """
    @brief save sumo config
    """
    # click over reference (to avoid problem with undo-redo)
    leftClick(referencePosition, 0, 0)
    # save sumo config using hotkey
    typeThreeKeys('ctrl', 'shift', 's')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.sumocfg")
    typeKey('enter')
    # wait for saving
    time.sleep(DELAY_SAVING)


def saveNetwork(referencePosition, useShortcut, clickOverReference=False, offsetX=0, offsetY=0, waitTime=2):
    """
    @brief save network
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, positions.reference, offsetX, offsetY)
    if (useShortcut):
        typeTwoKeys('ctrl', 's')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.saveNetwork):
            typeKey('down')
        typeKey('space')
    # wait for debug (due recomputing)
    time.sleep(waitTime)


def saveAdditionalElements(useShortcut, referencePosition, clickOverReference=False, waitTime=2):
    """
    @brief save additionals
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, 0, 0)
    if (useShortcut):
        typeThreeKeys('ctrl', 'shift', 'a')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.aditionalElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.aditionalElements.save):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)


def saveDemandElements(useShortcut, referencePosition, clickOverReference=False, offsetX=0, offsetY=0, waitTime=2):
    """
    @brief save routes
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, positions.reference, offsetX, offsetY)
    if (useShortcut):
        typeThreeKeys('ctrl', 'shift', 'd')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.demandElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.demandElements.save):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)


def saveDataElements(useShortcut, referencePosition, clickOverReference=False, offsetX=0, offsetY=0, waitTime=2):
    """
    @brief save datas
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, positions.reference, offsetX, offsetY)
    if (useShortcut):
        typeThreeKeys('ctrl', 'shift', 'b')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.dataElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.dataElements.save):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)


def saveMeanDatas(referencePosition, clickOverReference=False, offsetX=0, offsetY=0):
    """
    @brief save mean datas
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, positions.reference, offsetX, offsetY)
    # save datas using hotkey
    typeThreeKeys('ctrl', 'shift', 'm')


def saveAs(element):
    """
    @brief save the given element type as
    """
    # first obstain numberOfJumps and filename
    menuJumps = 0
    subMenuJumps = 0
    filename = ""
    if (element == "network"):
        menuJumps = attrs.toolbar.file.saveNetworkAs
        filename = "netAs.net.xml"
    elif (element == "additionals"):
        menuJumps = attrs.toolbar.file.aditionalElements.menu
        subMenuJumps = attrs.toolbar.file.aditionalElements.saveAs
        filename = "additionalsAs.add.xml"
    elif (element == "demands"):
        menuJumps = attrs.toolbar.file.demandElements.menu
        subMenuJumps = attrs.toolbar.file.demandElements.saveAs
        filename = "routesAs.rou.xml"
    elif (element == "datas"):
        menuJumps = attrs.toolbar.file.dataElements.menu
        subMenuJumps = attrs.toolbar.file.dataElements.saveAs
        filename = "datasAs.dat.xml"
    elif (element == "meanDatas"):
        menuJumps = attrs.toolbar.file.meanDataElements.menu
        subMenuJumps = attrs.toolbar.file.meanDataElements.saveAs
        filename = "datasAs.med.add.xml"
    # go to menu command
    typeTwoKeys('alt', 'f')
    for _ in range(menuJumps):
        typeKey('down')
    typeKey('space')
    for _ in range(subMenuJumps):
        typeKey('down')
    typeKey('space')
    # wait for open dialog
    time.sleep(1)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    # go to sandbox folder
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    # set filename
    updateText(filename)
    typeKey('enter')
    # wait for saving
    time.sleep(2)
