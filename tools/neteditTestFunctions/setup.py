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

# @file    setup.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import os
import sys
import time
import subprocess
import pyautogui
import pyperclip

from .input.keyboard import typeKey, keyRelease
from .constants import NETEDIT_APP, TEXTTEST_SANDBOX, REFERENCE_PNG
from .constants import DELAY_REFERENCE, DELAY_MOUSE_MOVE, DELAY_MOUSE_CLICK


def Popen(extraParameters):
    """
    @brief open netedit
    """
    # set the default parameters of Netedit
    neteditCall = [NETEDIT_APP]

    # check if a netedit config must be loaded
    if os.path.exists(os.path.join(TEXTTEST_SANDBOX, "netedit.netecfg")):
        neteditCall += ['-c netedit.netecfg']

    # add extra parameters
    neteditCall += extraParameters

    # return a subprocess with Netedit
    return subprocess.Popen(neteditCall, env=os.environ, stdout=sys.stdout, stderr=sys.stderr, cwd=TEXTTEST_SANDBOX)


def getReferenceMatch(neProcess, makeScrenshot):
    """
    @brief obtain reference referencePosition (pink square)
    """
    # show information
    print("Finding reference")
    # make a screenshot
    errorScreenshot = pyautogui.screenshot()
    try:
        # wait for reference
        time.sleep(DELAY_REFERENCE)
    # capture screen and search reference
        positionOnScreen = pyautogui.locateOnScreen(REFERENCE_PNG, minSearchTime=3, confidence=0.95)
    except Exception as e:
        # we cannot specify the exception here because some versions of pyautogui use one and some don't
        print(e)
        positionOnScreen = None
    # make a screenshot
        errorScreenshot = pyautogui.screenshot()
    # check if pos was found
    if positionOnScreen:
        # adjust position to center
        referencePosition = (positionOnScreen[0] + 16, positionOnScreen[1] + 16)
    # break loop
        print("TestFunctions: 'reference.png' found. Position: " +
              str(referencePosition[0]) + " - " + str(referencePosition[1]))
    # check that position is consistent (due scaling)
        if referencePosition != (304, 168):
            print("TestFunctions: Position of 'reference.png' isn't consistent")
    # click over position
        pyautogui.moveTo(referencePosition)
    # wait
        time.sleep(DELAY_MOUSE_MOVE)
    # press i for inspect mode
        typeKey("i")
    # click over position (used to center view in window)
        pyautogui.click(button='left')
    # wait after every operation
        time.sleep(DELAY_MOUSE_CLICK)
    # return reference position
        return referencePosition
    # referente not found, then write screenshot
    if (makeScrenshot):
        errorScreenshot.saveExistent("errorScreenshot.png")
    # kill netedit process
    neProcess.kill()
    # print debug information
    sys.exit("TestFunctions: Killed Netedit process. 'reference.png' not found")


def setupAndStart(extraParameters=[], makeScrenshot=True):
    """
    @brief setup and start netedit
    """
    if os.name == "posix":
        # to work around non working gtk clipboard
        pyperclip.set_clipboard("xclip")
    # Open Netedit
    neteditProcess = Popen(extraParameters)
    # atexit.register(quit, neteditProcess, False, False)
    # print debug information
    print("TestFunctions: Netedit opened successfully")
    # Release all keys
    keyRelease("shift")
    keyRelease("control")
    keyRelease("alt")
    # Wait for Netedit reference
    return neteditProcess, getReferenceMatch(neteditProcess, makeScrenshot)
