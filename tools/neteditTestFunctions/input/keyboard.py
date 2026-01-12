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

# @file    keyboard.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import pyautogui
import time
import pyperclip
from ..constants import DELAY_KEY

# Define the mapping from English to German keyboard layout
EN_KEYS = r"""y[];'\z/Y{}:"|Z<>?@#^&*()-_=+"""
DE_KEYS = u"""zü+öä#y-ZÜ*ÖÄ§Y,._"'^&()=ß?´¨"""
TRANS_TABLE = str.maketrans(EN_KEYS, DE_KEYS)


def typeKey(key):
    """
    @brief type single key
    """
    # type keys
    pyautogui.hotkey(key)
    # wait before every operation
    time.sleep(DELAY_KEY)


def keyPress(key):
    """
    @brief type single key press
    """
    # Leave key down
    pyautogui.keyDown(key)
    # wait after key down
    time.sleep(DELAY_KEY)


def keyRelease(key):
    """
    @brief type single key release
    """
    # Leave key up
    pyautogui.keyUp(key)
    # wait after key up
    time.sleep(DELAY_KEY)


def typeTwoKeys(key1, key2):
    """
    @brief type two keys at the same time (key1 -> key2)
    """
    # release key 1
    keyPress(key1)
    # type key 2
    typeKey(key2)
    # release key 1
    keyRelease(key1)


def typeThreeKeys(key1, key2, key3):
    """
    @brief type three keys at the same time (key1 -> key2 -> key3)
    """
    # press key 1
    keyPress(key1)
    # press key 1
    keyPress(key2)
    # type key 3
    typeKey(key3)
    # release key 2
    keyRelease(key2)
    # release key 1
    keyRelease(key1)


def translateKeys(value, layout="de"):
    """
    @brief translate keys between different keyboards
    """
    if layout == "de":
        return value.translate(TRANS_TABLE).encode("latin-1")
    return value


def updateText(newText, removePreviousContents=True, useClipboard=True, layout="de"):
    """
    @brief set the given new text in the focused textField/ComboBox/etc.
    """
    print(newText)
    # remove previous content
    if removePreviousContents:
        typeTwoKeys('ctrl', 'a')
    if useClipboard:
        # use copy & paste (due problems with certain characters, for example '|')
        pyperclip.copy(newText)
        pyautogui.hotkey('ctrl', 'v')
    else:
        pyautogui.typewrite(translateKeys(newText, layout))
