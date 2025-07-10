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

# @file    contextualMenu.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
import pyautogui
from ..constants import DELAY_MOUSE_MOVE, DELAY_KEY_TAB
from ..input.keyboard import typeKey


def contextualMenuOperation(referencePosition, position, contextualMenuOperation,
                            offsetX=0, offsetY=0):
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x + offsetX,
                       referencePosition[1] + position.y + offsetY]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='right')
    # place cursor over first operation
    for _ in range(contextualMenuOperation.mainMenuPosition):
        # wait before every down
        time.sleep(DELAY_KEY_TAB)
        # type down keys
        pyautogui.hotkey('down')
    # type space for select
    typeKey('space')
    # check if go to submenu A
    if contextualMenuOperation.subMenuAPosition > 0:
        # place cursor over second operation
        for _ in range(contextualMenuOperation.subMenuAPosition):
            # wait before every down
            time.sleep(DELAY_KEY_TAB)
            # type down keys
            pyautogui.hotkey('down')
        # type space for select
        typeKey('space')
        # check if go to submenu B
        if contextualMenuOperation.subMenuBPosition > 0:
            # place cursor over second operation
            for _ in range(contextualMenuOperation.subMenuBPosition):
                # wait before every down
                time.sleep(DELAY_KEY_TAB)
                # type down keys
                pyautogui.hotkey('down')
            # type space for select
            typeKey('space')
