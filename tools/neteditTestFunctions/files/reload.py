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
from ..general.functions import typeTwoKeys, waitQuestion


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
