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

# @file    quit.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
import pyautogui
import subprocess
from ..constants import DELAY_QUIT_NETEDIT
from ..general.functions import typeTwoKeys, waitQuestion
from ..input.keyboard import keyRelease


def quit(NeteditProcess, openNetDialog=False, saveNet=False,
         openAdditionalDialog=False, saveAdditionalElements=False,
         openDemandDialog=False, saveDemandElements=False,
         openDataDialog=False, saveDataElements=False,
         openMeanDataDialog=False, saveMeanDataElements=False):
    """
    @brief quit Netedit
    """
    # check if Netedit is already closed
    if NeteditProcess.poll() is not None:
        # print debug information
        print("[log] TestFunctions: Netedit already closed")
    else:
        # first move cursor out of magenta square
        pyautogui.moveTo(150, 200)
        # quit using hotkey
        typeTwoKeys('ctrl', 'q')
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
        # wait some seconds for netedit to quit
        if hasattr(subprocess, "TimeoutExpired"):
            try:
                NeteditProcess.wait(DELAY_QUIT_NETEDIT)
                print("TestFunctions: Netedit closed successfully")
                # all keys up
                keyRelease("shift")
                keyRelease("control")
                keyRelease("alt")
                # exit
                return
            except subprocess.TimeoutExpired:
                pass
        else:
            time.sleep(DELAY_QUIT_NETEDIT)
            if NeteditProcess.poll() is not None:
                print("TestFunctions: Netedit closed successfully")
                # all keys up
                keyRelease("shift")
                keyRelease("control")
                keyRelease("alt")
                # exit
                return
        # error closing NETEDIT then make a screenshot
        errorScreenshot = pyautogui.screenshot()
        errorScreenshot.saveExistent("errorScreenshot.png")
        # kill netedit
        NeteditProcess.kill()
        print("TestFunctions: Error closing Netedit")
        # all keys up
        keyRelease("shift")
        keyRelease("control")
        keyRelease("alt")
        # exit
        return
