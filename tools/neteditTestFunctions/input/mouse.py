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

# @file    mouse.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import time
import pyautogui
from ..constants import DELAY_MOUSE_MOVE, DELAY_MOUSE_CLICK, DELAY_DRAGDROP, DELAY_KEY
from ..input.keyboard import keyPress, keyRelease, typeKey


def leftClick(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square)
    """
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x, referencePosition[1] + position.y]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug
    print("TestFunctions: Clicked over position", clickedPosition[0], '-', clickedPosition[1])


def leftClickData(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) with an small offest y
    """
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x, referencePosition[1] + position.y + 30]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug (rest 30 to y position to unify output with internal tests)
    print("TestFunctions: Clicked over position", clickedPosition[0], '-', clickedPosition[1] - 30)


def leftClickOffset(referencePosition, position, offsetX, offsetY):
    """
    @brief do left click over a position relative to referencePosition (pink square)
    """
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x + offsetX, referencePosition[1] + position.y + offsetY]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug
    print("TestFunctions: Clicked over position", clickedPosition[0], '-', clickedPosition[1])


def leftClickShift(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) while shift key is pressed
    """
    # Leave Shift key pressed
    keyPress('shift')
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x, referencePosition[1] + position.y]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug
    print("TestFunctions: Clicked with Shift key pressed over position", clickedPosition[0], '-', clickedPosition[1])
    # Release Shift key
    keyRelease('shift')


def leftClickControl(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) while control key is pressed
    """
    # Leave Control key pressed
    keyPress('ctrl')
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x, referencePosition[1] + position.y]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug
    print("TestFunctions: Clicked with Control key pressed over position", clickedPosition[0], '-', clickedPosition[1])
    # Release Control key
    keyRelease('ctrl')


def leftClickAltShift(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) while alt key is pressed
    """
    # Leave alt key pressed
    keyPress('alt')
    # Leave shift key pressed
    keyPress('shift')
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x, referencePosition[1] + position.y]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug
    print("TestFunctions: Clicked with alt and shift key pressed over position",
          clickedPosition[0], '-', clickedPosition[1])
    # Release alt key
    keyRelease('alt')
    # Release shift key
    keyRelease('shift')


def rightClick(referencePosition, position, offsetX=0, offsetY=0):
    """
    @brief do right click over a position relative to referencePosition (pink square)
    """
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x + offsetX, referencePosition[1] + position.y + offsetY]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='right')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # show debug
    print("TestFunctions: Clicked over position", clickedPosition[0], '-', clickedPosition[1])


def leftClickMultiElement(referencePosition, position, underElement, offsetX=0, offsetY=0):
    """
    @brief do left click over a position relative to referencePosition (pink square) and selecting under element
    """
    # obtain clicked position
    clickedPosition = [referencePosition[0] + position.x + offsetX, referencePosition[1] + position.y + offsetY]
    # move mouse to position
    pyautogui.moveTo(clickedPosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # click over position
    pyautogui.click(button='left')
    # wait after every operation
    time.sleep(DELAY_MOUSE_CLICK)
    # go to element
    for _ in range(underElement + 1):
        typeKey('down')
    typeKey('space')
    print("TestFunctions: Clicked over position",
          clickedPosition[0], '-', clickedPosition[1], "under element", underElement)


def moveMouse(referencePosition, position, offsetX=0, offsetY=0):
    """
    @brief move mouse to the given position
    """
    # obtain clicked position
    movePosition = [referencePosition[0] + position.x + offsetX, referencePosition[1] + position.y + offsetY]
    # move mouse to position
    pyautogui.moveTo(movePosition)
    # wait after move
    time.sleep(DELAY_MOUSE_MOVE)
    # show debug
    print("TestFunctions: Moved to position", movePosition[0], '-', movePosition[1])


def dragDrop(referencePosition, x1, y1, x2, y2):
    """
    @brief drag and drop from position 1 to position 2
    """
    # wait before every operation
    time.sleep(DELAY_KEY)
    # obtain from and to position
    fromPosition = [referencePosition[0] + x1, referencePosition[1] + y1]
    tromPosition = [referencePosition[0] + x2, referencePosition[1] + y2]
    # move to from position
    pyautogui.moveTo(fromPosition)
    # wait before every operation
    time.sleep(DELAY_KEY)
    # drag mouse to X of 100, Y of 200 while holding down left mouse button
    pyautogui.dragTo(tromPosition[0], tromPosition[1], DELAY_DRAGDROP, button='left')
    # wait before every operation
    time.sleep(DELAY_KEY)
