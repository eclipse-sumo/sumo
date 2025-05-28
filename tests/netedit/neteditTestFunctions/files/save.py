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
    time.sleep(DELAY_SAVING)


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


def saveNetworkAs(waitTime=2):
    """
    @brief save network as
    """
    typeTwoKeys('alt', 'f')
    for _ in range(attrs.toolbar.file.saveNetworkAs):
        typeKey('down')
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("netAs.net.xml")
    typeKey('enter')
    # wait for saving
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


def saveAdditionalElementsAs(waitTime=2):
    """
    @brief save additional as
    """
    typeTwoKeys('alt', 'f')
    for _ in range(attrs.toolbar.file.aditionalElements.menu):
        typeKey('down')
    typeKey('space')
    for _ in range(attrs.toolbar.file.aditionalElements.saveAs):
        typeKey('down')
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additionalsAs.add.xml")
    typeKey('enter')
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


def saveDemandElementsAs(waitTime=2):
    """
    @brief save demand element as
    """
    typeTwoKeys('alt', 'f')
    for _ in range(attrs.toolbar.file.demandElements.menu):
        typeKey('down')
    typeKey('space')
    for _ in range(attrs.toolbar.file.demandElements.saveAs):
        typeKey('down')
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("routesAs.rou.xml")
    typeKey('enter')
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


def saveDataElementsAs(waitTime=2):
    """
    @brief save data element as
    """
    typeTwoKeys('alt', 'f')
    for _ in range(attrs.toolbar.file.dataElements.menu):
        typeKey('down')
    typeKey('space')
    for _ in range(attrs.toolbar.file.dataElements.saveAs):
        typeKey('down')
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datasAs.dat.xml")
    typeKey('enter')
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


def saveMeanDatasAs(waitTime=2):
    """
    @brief save data element as
    """
    typeTwoKeys('alt', 'f')
    for _ in range(attrs.toolbar.file.meanDataElements.menu):
        typeKey('down')
    typeKey('space')
    for _ in range(attrs.toolbar.file.meanDataElements.saveAs):
        typeKey('down')
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datasAs.med.add.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)