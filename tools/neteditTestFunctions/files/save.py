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
    @brief save existent element using shortcut (without opening a saving dialog)
    """
    filename = ""
    if (element == "network"):
        typeTwoKeys('ctrl', 's')
        filename = "new.net.net.xml"
    elif (element == "additionals"):
        typeThreeKeys('ctrl', 'shift', 'a')
        filename = "new.additionals.add.xml"
    elif (element == "demands"):
        typeThreeKeys('ctrl', 'shift', 'd')
        filename = "new.routes.rou.xml"
    elif (element == "datas"):
        typeThreeKeys('ctrl', 'shift', 'b')
        filename = "new.datas.dat.xml"
    elif (element == "meanDatas"):
        typeThreeKeys('ctrl', 'shift', 'm')
        filename = "new.meanDatas.add.xml"
    elif (element == "xml"):
        typeTwoKeys('ctrl', 'l')
        filename = "new.xml"
    elif (element == "sumoConfig"):
        typeThreeKeys('ctrl', 'shift', 's')
        filename = "new.config.sumocfg"
    elif (element == "neteditConfig"):
        typeThreeKeys('ctrl', 'shift', 'e')
        filename = "new.config.netecfg"
    # wait for open dialog
    time.sleep(2)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(TEXTTEST_SANDBOX)
    typeKey('enter')
    # set filename
    updateText(filename)
    typeKey('enter')
    # wait for saving
    time.sleep(2)


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
    time.sleep(2)


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
    time.sleep(2)
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
