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
from ..enums.attributesEnum import attrs
from ..constants import TEXTTEST_SANDBOX
from ..input.keyboard import typeKey, typeTwoKeys, typeThreeKeys, updateText


# declare delay in saving
_DELAY_OPENDIALOG = 1
_DELAY_SAVING = 2

# declare new files
_NETWORKFILE_NEW = "net.net.xml"
_ADDITIONALFILE_NEW = "additionals.add.xml"
_DEMANDFILE_NEW = "routes.rou.xml"
_DATAFILE_NEW = "datas.dat.xml"
_MEANDATA_NEW = "meanDatas.add.xml"
_PLAINXML_NEW = "net"
_SUMOCONFIG_NEW = "configNew.sumocfg"
_NETEDITCONFIG_NEW = "configNew.netecfg"

# declare saveAs files
_NETWORKFILE_SAVEAS = "netAs.net.xml"
_ADDITIONALFILE_SAVEAS = "additionalsAs.add.xml"
_DEMANDFILE_SAVEAS = "routesAs.rou.xml"
_DATAFILE_SAVEAS = "datasAs.dat.xml"
_MEANDATA_SAVEAS = "meanDatasAs.add.xml"
_SUMOCONFIG_SAVEAS = "configAs.sumocfg"
_NETEDITCONFIG_SAVEAS = "configAs.netecfg"


def saveNew(element):
    """
    @brief save new element (without opening a saving dialog)
    """
    # first obstain number of jumps
    menuJumps = 0
    subMenuJumps = 0
    filename = ""
    if (element == "network"):
        menuJumps = attrs.toolbar.file.saveNetwork
        filename = _NETWORKFILE_NEW
    elif (element == "additionals"):
        menuJumps = attrs.toolbar.file.aditionalElements.menu
        subMenuJumps = attrs.toolbar.file.aditionalElements.save
        filename = _ADDITIONALFILE_NEW
    elif (element == "demands"):
        menuJumps = attrs.toolbar.file.demandElements.menu
        subMenuJumps = attrs.toolbar.file.demandElements.save
        filename = _DEMANDFILE_NEW
    elif (element == "datas"):
        menuJumps = attrs.toolbar.file.dataElements.menu
        subMenuJumps = attrs.toolbar.file.dataElements.save
        filename = _DATAFILE_NEW
    elif (element == "meanDatas"):
        menuJumps = attrs.toolbar.file.meanDataElements.menu
        subMenuJumps = attrs.toolbar.file.meanDataElements.save
        filename = _PLAINXML_NEW
    elif (element == "sumoConfig"):
        menuJumps = attrs.toolbar.file.sumoConfig.menu
        subMenuJumps = attrs.toolbar.file.sumoConfig.save
        filename = _SUMOCONFIG_NEW
    elif (element == "neteditConfig"):
        menuJumps = attrs.toolbar.file.neteditConfig.menu
        subMenuJumps = attrs.toolbar.file.neteditConfig.save
        filename = _NETEDITCONFIG_NEW
    # go to menu command
    typeTwoKeys('alt', 'f')
    for _ in range(menuJumps):
        typeKey('down')
    typeKey('space')
    for _ in range(subMenuJumps):
        typeKey('down')
    # select space
    typeKey('space')
    # wait for open dialog
    time.sleep(_DELAY_OPENDIALOG)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    # set filename
    updateText(filename)
    typeKey('enter')
    # wait for saving
    time.sleep(_DELAY_SAVING)


def saveNewShortcut(element):
    """
    @brief save existent element using shortcut (without opening a saving dialog)
    """
    filename = ""
    if (element == "network"):
        typeTwoKeys('ctrl', 's')
        filename = _NETWORKFILE_NEW
    elif (element == "additionals"):
        typeThreeKeys('ctrl', 'shift', 'a')
        filename = _ADDITIONALFILE_NEW
    elif (element == "demands"):
        typeThreeKeys('ctrl', 'shift', 'd')
        filename = _DEMANDFILE_NEW
    elif (element == "datas"):
        typeThreeKeys('ctrl', 'shift', 'b')
        filename = _DATAFILE_NEW
    elif (element == "meanDatas"):
        typeThreeKeys('ctrl', 'shift', 'm')
        filename = _MEANDATA_NEW
    elif (element == "xml"):
        typeTwoKeys('ctrl', 'l')
        filename = _PLAINXML_NEW
    elif (element == "sumoConfig"):
        typeThreeKeys('ctrl', 'shift', 's')
        filename = _SUMOCONFIG_NEW
    elif (element == "neteditConfig"):
        typeThreeKeys('ctrl', 'shift', 'e')
        filename = _NETEDITCONFIG_NEW
    # wait for open dialog
    time.sleep(_DELAY_OPENDIALOG)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    # set filename
    updateText(filename)
    typeKey('enter')
    # wait for saving
    time.sleep(_DELAY_SAVING)


def saveExistent(element):
    """
    @brief save existent element (without opening a saving dialog)
    """
    # first obstain number of jumps
    menuJumps = 0
    subMenuJumps = 0
    if (element == "network"):
        menuJumps = attrs.toolbar.file.saveNetwork
    elif (element == "additionals"):
        menuJumps = attrs.toolbar.file.aditionalElements.menu
        subMenuJumps = attrs.toolbar.file.aditionalElements.save
    elif (element == "demands"):
        menuJumps = attrs.toolbar.file.demandElements.menu
        subMenuJumps = attrs.toolbar.file.demandElements.save
    elif (element == "datas"):
        menuJumps = attrs.toolbar.file.dataElements.menu
        subMenuJumps = attrs.toolbar.file.dataElements.save
    elif (element == "meanDatas"):
        menuJumps = attrs.toolbar.file.meanDataElements.menu
        subMenuJumps = attrs.toolbar.file.meanDataElements.save
    elif (element == "sumoConfig"):
        menuJumps = attrs.toolbar.file.sumoConfig.menu
        subMenuJumps = attrs.toolbar.file.sumoConfig.save
    elif (element == "neteditConfig"):
        menuJumps = attrs.toolbar.file.neteditConfig.menu
        subMenuJumps = attrs.toolbar.file.neteditConfig.save
    # go to menu command
    typeTwoKeys('alt', 'f')
    for _ in range(menuJumps):
        typeKey('down')
    typeKey('space')
    for _ in range(subMenuJumps):
        typeKey('down')
    # select space
    typeKey('space')
    # wait for saving
    time.sleep(_DELAY_SAVING)


def saveExistentShortcut(element):
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
    time.sleep(_DELAY_SAVING)


def saveAs(element):
    """
    @brief save the given element type as
    """
    # first obstain number of jumps and filename
    menuJumps = 0
    subMenuJumps = 0
    filename = ""
    if (element == "network"):
        menuJumps = attrs.toolbar.file.saveNetworkAs
        filename = _NETWORKFILE_SAVEAS
    elif (element == "additionals"):
        menuJumps = attrs.toolbar.file.aditionalElements.menu
        subMenuJumps = attrs.toolbar.file.aditionalElements.saveAs
        filename = _ADDITIONALFILE_SAVEAS
    elif (element == "demands"):
        menuJumps = attrs.toolbar.file.demandElements.menu
        subMenuJumps = attrs.toolbar.file.demandElements.saveAs
        filename = _DEMANDFILE_SAVEAS
    elif (element == "datas"):
        menuJumps = attrs.toolbar.file.dataElements.menu
        subMenuJumps = attrs.toolbar.file.dataElements.saveAs
        filename = _DATAFILE_SAVEAS
    elif (element == "meanDatas"):
        menuJumps = attrs.toolbar.file.meanDataElements.menu
        subMenuJumps = attrs.toolbar.file.meanDataElements.saveAs
        filename = _MEANDATA_SAVEAS
    elif (element == "sumoConfig"):
        menuJumps = attrs.toolbar.file.sumoConfig.menu
        subMenuJumps = attrs.toolbar.file.sumoConfig.saveAs
        filename = _SUMOCONFIG_SAVEAS
    elif (element == "neteditConfig"):
        menuJumps = attrs.toolbar.file.neteditConfig.menu
        subMenuJumps = attrs.toolbar.file.neteditConfig.saveAs
        filename = _NETEDITCONFIG_SAVEAS
    # go to menu command
    typeTwoKeys('alt', 'f')
    for _ in range(menuJumps):
        typeKey('down')
    typeKey('space')
    for _ in range(subMenuJumps):
        typeKey('down')
    typeKey('space')
    # wait for open dialog
    time.sleep(_DELAY_OPENDIALOG)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    # go to sandbox folder
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    # set filename
    updateText(filename)
    typeKey('enter')
    # wait for saving
    time.sleep(_DELAY_SAVING)
