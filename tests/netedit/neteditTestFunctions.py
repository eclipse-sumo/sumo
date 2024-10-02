# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    neteditTestFunctions.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

# Import libraries
from __future__ import print_function
import os
import sys
try:
    import subprocess32 as subprocess
except ImportError:
    import subprocess
import pyautogui
import time
import pyperclip
import attributesEnum as attrs  # noqa
import viewPositions as positions  # noqa
import contextualMenuOperations as contextualMenu  # noqa

# define delay before every operation
DELAY_KEY = 0.2
DELAY_DRAGDROP = 3
DELAY_KEY_TAB = 0.2
DELAY_MOUSE_MOVE = 0.5
DELAY_MOUSE_CLICK = 1
DELAY_QUESTION = 3
DELAY_SAVING = 1
DELAY_RELOAD = 3
DELAY_QUIT_NETEDIT = 5
DELAY_UNDOREDO = 1
DELAY_SELECT = 1
DELAY_RECOMPUTE = 3
DELAY_RECOMPUTE_VOLATILE = 5
DELAY_REMOVESELECTION = 2
DELAY_CHANGEMODE = 1
DELAY_REFERENCE = 15

_NETEDIT_APP = os.environ.get("NETEDIT_BINARY", "netedit")
_TEXTTEST_SANDBOX = os.environ.get("TEXTTEST_SANDBOX", os.getcwd())
_REFERENCE_PNG = os.path.join(os.path.dirname(__file__), "reference.png")

#################################################
# interaction functions
#################################################


def typeKeyUp(key):
    """
    @brief type single key up
    """
    # Leave key up
    pyautogui.keyUp(key)
    # wait after key up
    time.sleep(DELAY_KEY)


def typeKeyDown(key):
    """
    @brief type single key down
    """
    # Leave key down
    pyautogui.keyDown(key)
    # wait after key down
    time.sleep(DELAY_KEY)


def typeEscape():
    """
    @brief type escape key
    """
    # type ESC key
    typeKey('esc')


def typeEnter():
    """
    @brief type enter key
    """
    # type enter key
    typeKey('enter')


def typeSpace():
    """
    @brief type space key
    """
    # type space key
    typeKey('space')


def typeDelete():
    """
    @brief type delete key
    """
    # type space key
    typeKey('delete')


def typeTab():
    """
    @brief type tab key
    """
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type keys
    pyautogui.hotkey('tab')


def typeUp():
    """
    @brief type up key
    """
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type key
    pyautogui.hotkey('up')


def typeDown():
    """
    @brief type down key
    """
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type key
    pyautogui.hotkey('down')


def typeLeft():
    """
    @brief type left key
    """
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type key
    pyautogui.hotkey('left')


def typeRight():
    """
    @brief type right key
    """
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type key
    pyautogui.hotkey('right')


def typeBackspace():
    """
    @brief type backspace key
    """
    # wait before every operation
    time.sleep(DELAY_KEY)
    # type keys
    pyautogui.hotkey('backspace')


def typeInvertTab():
    """
    @brief type Shift + Tab keys
    """
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type two keys at the same time
    pyautogui.hotkey('shift', 'tab')


def typeKey(key):
    """
    @brief type single key
    """
    # type keys
    pyautogui.hotkey(key)
    # wait before every operation
    time.sleep(DELAY_KEY)


def typeTwoKeys(key1, key2):
    """
    @brief type two keys at the same time (key1 -> key2)
    """
    # press key 1
    typeKeyDown(key1)
    # type key 2
    typeKey(key2)
    # leave key 1
    typeKeyUp(key1)


def typeThreeKeys(key1, key2, key3):
    """
    @brief type three keys at the same time (key1 -> key2 -> key3)
    """
    # press key 1
    typeKeyDown(key1)
    # type key 2 and 3
    typeTwoKeys(key2, key3)
    # leave key 1
    typeKeyUp(key1)


def translateKeys(value, layout="de"):
    """
    @brief translate keys between different keyboards
    """
    tr = {}
    if layout == "de":
        en = r"""y[];'\z/Y{}:"|Z<>?@#^&*()-_=+§"""
        de = u"""zü+öä#y-ZÜ*ÖÄ'Y;:_"§&/()=ß?´`^"""
        # join as keys and values
        tr.update(dict(zip(en, de)))
    return "".join(map(lambda x: tr.get(x, x), value))


def pasteIntoTextField(value, removePreviousContents=True, useClipboard=True, layout="de"):
    """
    @brief paste value into current text field
    """
    print(value)
    # remove previous content
    if removePreviousContents:
        typeTwoKeys('ctrl', 'a')
    if useClipboard:
        # use copy & paste (due problems with certain characters, for example '|')
        pyperclip.copy(value)
        pyautogui.hotkey('ctrl', 'v')
    else:
        pyautogui.typewrite(translateKeys(value, layout))


def leftClick(referencePosition, position, offsetX=0, offsetY=0):
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
    print("TestFunctions: Clicked over position", clickedPosition[0], '-', clickedPosition[1])


def leftClickShift(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) while shift key is pressed
    """
    # Leave Shift key pressed
    typeKeyDown('shift')
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
    typeKeyUp('shift')


def leftClickControl(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) while control key is pressed
    """
    # Leave Control key pressed
    typeKeyDown('ctrl')
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
    typeKeyUp('ctrl')


def leftClickAltShift(referencePosition, position):
    """
    @brief do left click over a position relative to referencePosition (pink square) while alt key is pressed
    """
    # Leave alt key pressed
    typeKeyDown('alt')
    # Leave shift key pressed
    typeKeyDown('shift')
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
    typeKeyUp('alt')
    # Release shift key
    typeKeyUp('shift')


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
        typeDown()
    typeSpace()
    print("TestFunctions: Clicked over position",
          clickedPosition[0], '-', clickedPosition[1], "under element", underElement)

#################################################
    # basic functions
#################################################


def Popen(extraParameters):
    """
    @brief open netedit
    """
    # set the default parameters of Netedit
    neteditCall = [_NETEDIT_APP]

    # check if a netedit config must be loaded
    if os.path.exists(os.path.join(_TEXTTEST_SANDBOX, "netedit.netecfg")):
        neteditCall += ['-c', os.path.join(_TEXTTEST_SANDBOX, "netedit.netecfg")]

    # add extra parameters
    neteditCall += extraParameters

    # return a subprocess with Netedit
    return subprocess.Popen(neteditCall, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)


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
        positionOnScreen = pyautogui.locateOnScreen(_REFERENCE_PNG, minSearchTime=3, confidence=0.95)
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
        errorScreenshot.save("errorScreenshot.png")
    # kill netedit process
    neProcess.kill()
    # print debug information
    sys.exit("TestFunctions: Killed Netedit process. 'reference.png' not found")


def setupAndStart(testRoot, extraParameters=[], makeScrenshot=True):
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
    # all keys up
    typeKeyUp("shift")
    typeKeyUp("control")
    typeKeyUp("alt")
    # Wait for Netedit reference
    return neteditProcess, getReferenceMatch(neteditProcess, makeScrenshot)


def supermodeNetwork():
    """
    @brief select supermode Network
    """
    typeKey('F2')


def supermodeDemand():
    """
    @brief select supermode Demand
    """
    typeKey('F3')
    # wait for output
    time.sleep(DELAY_RECOMPUTE)


def supermodeData():
    """
    @brief select supermode Data
    """
    typeKey('F4')
    # wait for output
    time.sleep(DELAY_RECOMPUTE)


def rebuildNetwork():
    """
    @brief rebuild network
    """
    typeKey('F5')
    # wait for output
    time.sleep(DELAY_RECOMPUTE)


def rebuildNetworkWithVolatileOptions(question=True):
    """
    @brief rebuild network with volatile options
    """
    typeTwoKeys('shift', 'F5')
    # confirm recompute
    if question is True:
        waitQuestion('y')
    # wait for output
        time.sleep(DELAY_RECOMPUTE_VOLATILE)
    else:
        waitQuestion('n')


def joinSelectedJunctions():
    """
    @brief join selected junctions
    """
    typeKey('F7')


def focusOnFrame():
    """
    @brief select focus on upper element of current frame
    """
    typeTwoKeys('shift', 'F12')
    time.sleep(1)


def undo(referencePosition, number, offsetX=0, offsetY=0):
    """
    @brief undo last operation
    """
    # first wait
    time.sleep(DELAY_UNDOREDO)
    # focus current frame
    focusOnFrame()
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey('i')
    # click over referencePosition
    leftClick(referencePosition, positions.reference, offsetX, offsetY)
    for _ in range(number):
        typeTwoKeys('ctrl', 'z')
        time.sleep(DELAY_UNDOREDO)


def redo(referencePosition, number, offsetX=0, offsetY=0):
    """
    @brief undo last operation
    """
    # first wait
    time.sleep(DELAY_UNDOREDO)
    # focus current frame
    focusOnFrame()
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey('i')
    # click over referencePosition
    leftClick(referencePosition, positions.reference, offsetX, offsetY)
    for _ in range(number):
        typeTwoKeys('ctrl', 'y')
        time.sleep(DELAY_UNDOREDO)


def loadViewPort():
    """
    @brief load viewport
    """
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # go to load
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("loadViewport.xml")
    typeEnter()
    # wait
    time.sleep(DELAY_SELECT)
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


def saveViewPort():
    """
    @brief save viewport
    """
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # go to save
    typeTab()
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("viewport.xml")
    typeEnter()
    # wait
    time.sleep(DELAY_SELECT)
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


def setViewport(zoom, x, y, z, r):
    """
    @brief edit viewport
    """
    # open edit viewport dialog
    typeTwoKeys('ctrl', 'i')
    # go to zoom
    for _ in range(2):
        typeTab()
    # Paste X
    if (len(zoom) > 0):
        pasteIntoTextField(zoom)
    # go to Y
    typeTab()
    # Paste X
    if (len(x) > 0):
        pasteIntoTextField(x)
    # go to Y
    typeTab()
    # Paste Y
    if (len(y) > 0):
        pasteIntoTextField(y)
    # go to Z
    typeTab()
    # Paste Z
    if (len(z) > 0):
        pasteIntoTextField(z)
    # go to rotation
    typeTab()
    # Paste rotation
    if (len(r) > 0):
        pasteIntoTextField(r)
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


def waitQuestion(answer):
    """
    @brief wait question of Netedit and select a yes/no answer
    """
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    # Answer can be "y" or "n"
    typeTwoKeys('alt', answer)


def reload(NeteditProcess, openNetNonSavedDialog=False, saveNet=False,
           openAdditionalsNonSavedDialog=False, saveAdditionals=False,
           openDemandNonSavedDialog=False, saveDemandElements=False,
           openDataNonSavedDialog=False, saveDataElements=False,
           openMeanDataNonSavedDialog=False, saveMeanDataElements=False):
    """
    @brief reload Netedit
    """
    # first move cursor out of magenta square
    pyautogui.moveTo(150, 200)
    # reload using hotkey
    typeTwoKeys('ctrl', 'r')
    # Check if net must be saved
    if openNetNonSavedDialog:
        # Wait some seconds
        time.sleep(DELAY_QUESTION)
        if saveNet:
            waitQuestion('s')
        # wait for log
            time.sleep(DELAY_RECOMPUTE)
        else:
            waitQuestion('d')
    # Check if additionals must be saved
    if openAdditionalsNonSavedDialog:
        # Wait some seconds
        time.sleep(DELAY_QUESTION)
        if saveAdditionals:
            waitQuestion('s')
        else:
            waitQuestion('d')
    # Check if demand elements must be saved
    if openDemandNonSavedDialog:
        # Wait some seconds
        time.sleep(DELAY_QUESTION)
        if saveDemandElements:
            waitQuestion('s')
        else:
            waitQuestion('d')
    # Check if data elements must be saved
    if openDataNonSavedDialog:
        # Wait some seconds
        time.sleep(DELAY_QUESTION)
        if saveDataElements:
            waitQuestion('s')
        else:
            waitQuestion('d')
    # Check if meanData elements must be saved
    if openMeanDataNonSavedDialog:
        # Wait some seconds
        time.sleep(DELAY_QUESTION)
        if saveMeanDataElements:
            waitQuestion('s')
        else:
            waitQuestion('d')
    # Wait some seconds
    time.sleep(DELAY_RELOAD)
    # check if Netedit was crashed during reloading
    if NeteditProcess.poll() is not None:
        print("TestFunctions: Error reloading Netedit")


def quit(NeteditProcess, openNetNonSavedDialog=False, saveNet=False,
         openAdditionalsNonSavedDialog=False, saveAdditionals=False,
         openDemandNonSavedDialog=False, saveDemandElements=False,
         openDataNonSavedDialog=False, saveDataElements=False,
         openMeanDataNonSavedDialog=False, saveMeanDataElements=False):
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
        if openNetNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveNet:
                waitQuestion('s')
                # wait for log
                time.sleep(DELAY_RECOMPUTE)
            else:
                waitQuestion('d')
        # Check if additionals must be saved
        if openAdditionalsNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveAdditionals:
                waitQuestion('s')
            else:
                waitQuestion('d')
        # Check if demand elements must be saved
        if openDemandNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveDemandElements:
                waitQuestion('s')
            else:
                waitQuestion('d')
        # Check if data elements must be saved
        if openDataNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveDataElements:
                waitQuestion('s')
            else:
                waitQuestion('d')
        # Check if meanData elements must be saved
        if openMeanDataNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveMeanDataElements:
                waitQuestion('s')
            else:
                waitQuestion('q')
        # wait some seconds for netedit to quit
        if hasattr(subprocess, "TimeoutExpired"):
            try:
                NeteditProcess.wait(DELAY_QUIT_NETEDIT)
                print("TestFunctions: Netedit closed successfully")
                # all keys up
                typeKeyUp("shift")
                typeKeyUp("control")
                typeKeyUp("alt")
                # exit
                return
            except subprocess.TimeoutExpired:
                pass
        else:
            time.sleep(DELAY_QUIT_NETEDIT)
            if NeteditProcess.poll() is not None:
                print("TestFunctions: Netedit closed successfully")
                # all keys up
                typeKeyUp("shift")
                typeKeyUp("control")
                typeKeyUp("alt")
                # exit
                return
        # error closing NETEDIT then make a screenshot
        errorScreenshot = pyautogui.screenshot()
        errorScreenshot.save("errorScreenshot.png")
        # kill netedit
        NeteditProcess.kill()
        print("TestFunctions: Error closing Netedit")
        # all keys up
        typeKeyUp("shift")
        typeKeyUp("control")
        typeKeyUp("alt")
        # exit
        return


def openNetworkAs(waitTime=2):
    """
    @brief load network using dialog
    """
    # open save network as dialog
    typeTwoKeys('ctrl', 'o')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("config.net.xml")
    typeEnter()
    # wait for saving
    time.sleep(waitTime)


def saveNetwork(referencePosition, clickOverReference=False, offsetX=0, offsetY=0):
    """
    @brief save network
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, positions.reference, offsetX, offsetY)
    # save network using hotkey
    typeTwoKeys('ctrl', 's')
    # wait for debug (due recomputing)
    time.sleep(DELAY_RECOMPUTE)


def saveNetworkAs(waitTime=2):
    """
    @brief save network as
    """
    # open save network as dialog
    typeThreeKeys('ctrl', 'shift', 's')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("net.net.xml")
    typeEnter()
    # wait for saving
    time.sleep(waitTime)
    # wait for debug
    time.sleep(DELAY_RECOMPUTE)


def saveAdditionals(referencePosition, clickOverReference=False):
    """
    @brief save additionals
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, 0, 0)
    # save additionals using hotkey
    typeThreeKeys('ctrl', 'shift', 'a')


def saveRoutes(referencePosition, clickOverReference=True):
    """
    @brief save routes
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, 0, 0)
    # save routes using hotkey
    typeThreeKeys('ctrl', 'shift', 'd')


def saveDatas(referencePosition, clickOverReference=True, offsetX=0, offsetY=0):
    """
    @brief save datas
    """
    # check if clickOverReference is enabled
    if clickOverReference:
        # click over reference (to avoid problem with undo-redo)
        leftClick(referencePosition, positions.reference, offsetX, offsetY)
    # save datas using hotkey
    typeThreeKeys('ctrl', 'shift', 'b')


def fixDemandElements(solution):
    """
    @brief fix stoppingPlaces
    """
    # select bullet depending of solution
    if (solution == "saveInvalids"):
        for _ in range(3):
            typeInvertTab()
        typeSpace()
        # go back and press accept
        for _ in range(3):
            typeTab()
        typeSpace()
    elif (solution == "fixPositions"):
        for _ in range(2):
            typeInvertTab()
        typeSpace()
        # go back and press accept
        for _ in range(2):
            typeTab()
        typeSpace()
    elif (solution == "selectInvalids"):
        typeInvertTab()
        typeSpace()
        # go back and press accept
        typeTab()
        typeSpace()
    elif (solution == "activateFriendlyPos"):
        # default option, then press accept
        typeSpace()
    else:
        # press cancel
        typeTab()
        typeSpace()


def openAboutDialog(waitingTime=DELAY_QUESTION):
    """
    @brief open and close about dialog
    """
    # type F12 to open about dialog
    typeKey('F12')
    # wait before closing
    time.sleep(waitingTime)
    # press enter to close dialog (Ok must be focused)
    typeSpace()


def openNeteditConfigShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("netedit_open.netecfg")
    typeEnter()
    # wait for loading
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
        typeDown()
    typeRight()
    typeDown()
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("saveConfigAs.netecfg")
    typeEnter()
    # wait for loading
    time.sleep(waitTime)


def openNetworkShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("config.net.xml")
    typeEnter()
    # wait for loading
    time.sleep(waitTime)


def openConfigurationShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeThreeKeys('ctrl', 'shift', 'o')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("config.netccfg")
    typeEnter()
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
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("net")
    typeEnter()
    # wait for loading
    time.sleep(waitTime)


def changeEditMode(key):
    """
    @brief Change edit mode (alt+1-9)
    """
    typeTwoKeys('alt', key)

#################################################
    # Configs
#################################################


def openNeteditConfigAs(waitTime=2):
    """
    @brief load netedit config using dialog
    """
    # open save network as dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("config.netecfg")
    typeEnter()
    # wait for saving
    time.sleep(waitTime)


def openSumoConfigAs(referencePosition):
    """
    @brief load netedit config using dialog
    """
    # click over reference (to avoid problem with undo-redo)
    leftClick(referencePosition, 0, 0)
    # open save network as dialog
    typeTwoKeys('ctrl', 'm')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("config.sumocfg")
    typeEnter()
    # wait for saving
    time.sleep(DELAY_SAVING)


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
    pasteIntoTextField(_TEXTTEST_SANDBOX)
    typeEnter()
    pasteIntoTextField("config.sumocfg")
    typeEnter()
    # wait for saving
    time.sleep(DELAY_SAVING)

#################################################
    # Create nodes and edges
#################################################


def createEdgeMode():
    """
    @brief Change to create edge mode
    """
    typeKey('e')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def cancelEdge():
    """
    @brief Cancel current created edge (used in chain mode)
    """
    # type ESC to cancel current edge
    typeEscape()

#################################################
    # Inspect mode
#################################################


def inspectMode():
    """
    @brief go to inspect mode
    """
    typeKey('i')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def modifyAttribute(attributeNumber, value, overlapped):
    """
    @brief modify attribute of type int/float/string
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeNumber + 1 + attrs.editElements.overlapped):
            typeTab()
    else:
        for _ in range(attributeNumber + 1):
            typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type Enter to commit change
    typeEnter()


def modifyBoolAttribute(attributeNumber, overlapped):
    """
    @brief modify boolean attribute
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeNumber + 1 + attrs.editElements.overlapped):
            typeTab()
    else:
        for _ in range(attributeNumber + 1):
            typeTab()
    # type SPACE to change value
    typeSpace()


def modifyColorAttribute(attributeNumber, color, overlapped):
    """
    @brief modify color using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeNumber + 1 + attrs.editElements.overlapped):
            typeTab()
    else:
        for _ in range(attributeNumber + 1):
            typeTab()
    typeSpace()
    # go to list of colors TextField
    for _ in range(2):
        typeInvertTab()
    # select color
    for _ in range(1 + color):
        typeKey('down')
    # go to accept button and press it
    typeTab()
    typeSpace()


def modifyAttributeVClassDialog(attribute, vClass, overlapped, disallowAll=True, cancel=False, reset=False):
    """
    @brief modify vclass attribute using dialog
    """
    # open dialog
    modifyBoolAttribute(attribute, overlapped)
    # first check if disallow all
    if (disallowAll):
        for _ in range(attrs.dialog.allowVClass.disallowAll):
            typeTab()
        typeSpace()
        # go to vClass
        for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
            typeTab()
        # Change current value
        typeSpace()
    else:
        # go to vClass
        for _ in range(vClass):
            typeTab()
        # Change current value
        typeSpace()
    # check if cancel
    if (cancel):
        for _ in range(attrs.dialog.allowVClass.cancel - vClass):
            typeTab()
        typeSpace()
    elif (reset):
        for _ in range(attrs.dialog.allowVClass.reset - vClass):
            typeTab()
        typeSpace()
        for _ in range(2):
            typeInvertTab()
        typeSpace()
    else:
        for _ in range(attrs.dialog.allowVClass.accept - vClass):
            typeTab()
        typeSpace()


def checkUndoRedo(referencePosition, offsetX=0, offsetY=0):
    """
    @brief Check undo-redo
    """
    # Check undo
    undo(referencePosition, 9, offsetX)
    # Check redo
    redo(referencePosition, 9, offsetY)


def checkParameters(referencePosition, attributeNumber, overlapped, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeNumber, "dummyGenericParameters", overlapped)
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeNumber, "key1|key2|key3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeNumber, "key1=value1|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeNumber, "key1=|key2=|key3=", overlapped)
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeNumber, "", overlapped)
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeNumber, "key1duplicated=value1|key1duplicated=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeNumber, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", overlapped)
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeNumber, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeNumber, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeNumber, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3", overlapped)
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)


def checkDoubleParameters(referencePosition, attributeNumber, overlapped, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeNumber, "dummyGenericParameters", overlapped)
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeNumber, "key1|key2|key3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeNumber, "key1=1|key2=2|key3=3", overlapped)
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeNumber, "key1=|key2=|key3=", overlapped)
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeNumber, "", overlapped)
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeNumber, "key1duplicated=1|key1duplicated=2|key3=3", overlapped)
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeNumber, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", overlapped)
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeNumber, "keyInvalid.;%>%$$=1|key2=2|key3=3", overlapped)
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeNumber, "key1=valueInvalid%;%$<>$$%|key2=2|key3=3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeNumber, "keyFinal1=1|keyFinal2=2|keyFinal3=3", overlapped)
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)

#################################################
    # Move mode
#################################################


def moveMode():
    """
    @brief set move mode
    """
    typeKey('m')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def moveElementHorizontal(referencePosition, originalPosition, radius):
    """
    @brief move element in horizontal
    """
    leftClick(referencePosition, originalPosition)
    # move element
    dragDrop(referencePosition, originalPosition.x, originalPosition.y,
             originalPosition.x + radius.right, originalPosition.y)
    dragDrop(referencePosition, originalPosition.x + radius.right,
             originalPosition.y, originalPosition.x + radius.left, originalPosition.y)


def moveElementVertical(referencePosition, originalPosition, radius):
    """
    @brief move element in vertical
    """
    # move element
    if (radius.up != 0):
        dragDrop(referencePosition, originalPosition.x, originalPosition.y,
                 originalPosition.x, originalPosition.y + radius.up)
    if (radius.down != 0):
        dragDrop(referencePosition, originalPosition.x, originalPosition.y + radius.up,
                 originalPosition.x, originalPosition.y + radius.down)


def moveElement(referencePosition, originalPosition, radius):
    """
    @brief move element
    """
    # move element
    dragDrop(referencePosition,
             originalPosition.x,
             originalPosition.y,
             originalPosition.x + radius.right,
             originalPosition.y)
    dragDrop(referencePosition,
             originalPosition.x + radius.right,
             originalPosition.y,
             originalPosition.x + radius.right,
             originalPosition.y + radius.down)
    dragDrop(referencePosition,
             originalPosition.x + radius.right,
             originalPosition.y + radius.down,
             originalPosition.x + radius.left,
             originalPosition.y + radius.down)
    dragDrop(referencePosition,
             originalPosition.x + radius.left,
             originalPosition.y + radius.down,
             originalPosition.x + radius.left,
             originalPosition.y + radius.up)

#################################################
    # crossings
#################################################


def crossingMode():
    """
    @brief Change to crossing mode
    """
    typeKey('r')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def createCrossing(hasTLS):
    """
    @brief create crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to create crossing button depending of hasTLS
    if hasTLS:
        for _ in range(attrs.crossing.createTLS.button):
            typeTab()
    else:
        for _ in range(attrs.crossing.create.button):
            typeTab()
    # type space to create crossing
    typeSpace()


def modifyCrossingDefaultValue(numtabs, value):
    """
    @brief change default int/real/string crossing default value
    """
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + attrs.crossing.firstField):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def modifyCrossingDefaultBoolValue(numtabs):
    """
    @brief change default boolean crossing default value
    """
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + attrs.crossing.firstField):
        typeTab()
    # type space to change value
    typeSpace()


def crossingClearEdges():
    """
    @brief clear crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to clear button
    for _ in range(attrs.crossing.clearEdges):
        typeTab()
    # type space to activate button
    typeSpace()


def crossingInvertEdges():
    """
    @brief invert crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to invert button
    for _ in range(attrs.crossing.invertEdges):
        typeTab()
    # type space to activate button
    typeSpace()

#################################################
    # Connection mode
#################################################


def connectionMode():
    """
    @brief Change to connection mode
    """
    typeKey('c')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def createConnection(referencePosition, fromLanePosition, toLanePosition, mode=""):
    """
    @brief create connection
    """
    # check if connection has to be created in certain mode
    if mode == "conflict":
        typeKeyDown('ctrl')
    elif mode == "yield":
        typeKeyDown('shift')
    # select first lane
    leftClick(referencePosition, fromLanePosition)
    # select another lane for create a connection
    leftClick(referencePosition, toLanePosition)
    # check if connection has to be created in certain mode
    if mode == "conflict":
        typeKeyUp('ctrl')
    elif mode == "yield":
        typeKeyUp('shift')


def saveConnectionEdit():
    """
    @brief Change to crossing mode
    """
    # focus current frame
    focusOnFrame()
    # go to cancel button
    for _ in range(attrs.connection.saveConnections):
        typeTab()
    # type space to press button
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)

#################################################
    # additionals
#################################################


def additionalMode():
    """
    @brief change to additional mode
    """
    typeKey('a')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def changeElement(element):
    """
    @brief change element (Additional, shape, vehicle...)
    """
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    for _ in range(attrs.additionals.changeElement):
        typeTab()
    # paste the new value
    pasteIntoTextField(element)
    # type enter to save change
    typeEnter()


def changeDefaultValue(numTabs, value):
    """
    @brief modify default int/double/string value of an additional, shape, vehicle...
    """
    # focus current frame
    focusOnFrame()
    # go to value TextField
    for _ in range(numTabs):
        typeTab()
    # paste new value
    pasteIntoTextField(value)
    # type enter to save new value
    typeEnter()


def changeDefaultBoolValue(numTabs):

    # focus current frame
    focusOnFrame()
    # place cursor in check Box position
    for _ in range(numTabs):
        typeTab()
    # Change current value
    typeSpace()


def changeDefaultAllowDisallowValue(numTabs):
    """
    @brief modify allow/disallow values
    """
    # open dialog
    changeDefaultBoolValue(numTabs)
    # select vtypes
    for _ in range(2):
        typeTab()
    # Change current value
    typeSpace()
    # select vtypes
    for _ in range(6):
        typeTab()
    # Change current value
    typeSpace()
    # select vtypes
    for _ in range(12):
        typeTab()
    # Change current value
    typeSpace()
    # select vtypes
    for _ in range(11):
        typeTab()
    # Change current value
    typeSpace()


def selectAdditionalChild(numTabs, childNumber):
    """
    @brief select child of additional
    """
    # focus current frame
    focusOnFrame()
    # place cursor in the list of childs
    for _ in range(numTabs + 1):
        typeTab()
    # select child
    for _ in range(childNumber):
        typeKey('down')
    typeSpace()
    # use TAB to select additional child
    typeTab()


def fixStoppingPlace(solution):
    """
    @brief fix stoppingPlaces
    """
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    # select bullet depending of solution
    if (solution == "saveInvalids"):
        for _ in range(3):
            typeInvertTab()
        typeSpace()
    # go back and press accept
        for _ in range(3):
            typeTab()
        typeSpace()
    elif (solution == "fixPositions"):
        for _ in range(2):
            typeInvertTab()
        typeSpace()
    # go back and press accept
        for _ in range(2):
            typeTab()
        typeSpace()
    elif (solution == "selectInvalids"):
        typeInvertTab()
        typeSpace()
    # go back and press accept
        typeTab()
        typeSpace()
    elif (solution == "activateFriendlyPos"):
        # default option, then press accept
        typeSpace()
    else:
        # press cancel
        typeTab()
        typeSpace()

#################################################
    # demand elements
#################################################


def routeMode():
    """
    @brief change to route mode
    """
    typeKey('r')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def changeRouteMode(value):
    """
    @brief change route mode
    """
    # focus current frame
    focusOnFrame()
    # jump to route mode
    for _ in range(2):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def changeRouteVClass(value):
    """
    @brief change vClass mode
    """
    # focus current frame
    focusOnFrame()
    # jump to vClass
    for _ in range(4):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def fixDemandElement(value):
    """
    @brief fix demand element
    """
    # focus current frame
    focusOnFrame()
    # jump to option
    for _ in range(value):
        typeInvertTab()
    # type space to select
    typeSpace()
    # accept
    typeTwoKeys('alt', 'a')

#################################################
    # person elements
#################################################


def personMode():
    """
    @brief change to person mode
    """
    typeKey('p')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def changePersonMode(value):
    """
    @brief change person mode
    """
    # focus current frame
    focusOnFrame()
    # jump to person mode
    typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def changePersonVClass(value):
    """
    @brief change vClass mode
    """
    # focus current frame
    focusOnFrame()
    # jump to vClass
    for _ in range(3):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def changePersonPlan(personPlan, flow):
    """
    @brief change personPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to person plan
    if (flow):
        for _ in range(23):
            typeTab()
    else:
        for _ in range(16):
            typeTab()
    # paste the new personPlan
    pasteIntoTextField(personPlan)
    # type enter to save change
    typeEnter()

#################################################
    # container elements
#################################################


def containerMode():
    """
    @brief change to container mode
    """
    typeKey('c')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def changeContainerMode(value):
    """
    @brief change container mode
    """
    # focus current frame
    focusOnFrame()
    # jump to container mode
    typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def changeContainerVClass(value):
    """
    @brief change vClass mode
    """
    # focus current frame
    focusOnFrame()
    # jump to vClass
    for _ in range(3):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


def changeContainerPlan(containerPlan, flow):
    """
    @brief change containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to container plan
    if (flow):
        for _ in range(23):
            typeTab()
    else:
        for _ in range(16):
            typeTab()
    # paste the new containerPlan
    pasteIntoTextField(containerPlan)
    # type enter to save change
    typeEnter()

#################################################
    # personPlan elements
#################################################


def personPlanMode():
    """
    @brief change to person mode
    """
    typeKey('l')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def changePersonPlanMode(personPlan):
    """
    @brief change containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to person plan
    for _ in range(5):
        typeTab()
    # paste the new containerPlan
    pasteIntoTextField(personPlan)
    # type enter to save change
    typeEnter()


def selectPerson(person):
    """
    @brief select person in containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to person plan
    for _ in range(2):
        typeTab()
    # paste the new containerPlan
    pasteIntoTextField(person)
    # type enter to save change
    typeEnter()

#################################################
    # containerPlan elements
#################################################


def containerPlanMode():
    """
    @brief change to person mode
    """
    typeKey('h')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def changeContainerPlanMode(containerPlan):
    """
    @brief change containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to container plan
    for _ in range(5):
        typeTab()
    # paste the new containerPlan
    pasteIntoTextField(containerPlan)
    # type enter to save change
    typeEnter()

#################################################
    # stop elements
#################################################


def stopMode():
    """
    @brief change to person mode
    """
    typeKey('a')


def changeStopParent(stopParent):
    """
    @brief change stop parent
    """
    # focus current frame
    focusOnFrame()
    for _ in range(2):
        typeTab()
    # paste the new stop parent
    pasteIntoTextField(stopParent)
    # type enter to save change
    typeEnter()


def changeStopType(stopType):
    """
    @brief change stop type
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(5):
        typeTab()
    # paste the new personPlan
    pasteIntoTextField(stopType)
    # type enter to save change
    typeEnter()

#################################################
    # vehicle elements
#################################################


def vehicleMode():
    """
    @brief change to vehicle mode
    """
    typeKey('v')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)

#################################################
    # vType elements
#################################################


def typeMode():
    """
    @brief change to type mode
    """
    typeKey('t')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def createVType():
    """
    @brief create vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.create):
        typeTab()
    # type space
    typeSpace()


def deleteVType():
    """
    @brief delete vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.delete):
        typeTab()
    # type space
    typeSpace()


def copyVType():
    """
    @brief copy vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.copy):
        typeTab()
    # type space
    typeSpace()


def openVTypeDialog():
    """
    @brief create vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.dialog):
        typeTab()
    # type space
    typeSpace()
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)


def closeVTypeDialog():
    """
    @brief close vType dialog saving elements
    """
    typeTwoKeys('alt', 'a')


def modifyVTypeAttribute(attributeNumber, value):
    """
    @brief modify VType attribute of type int/float/string
    """
    # focus dialog
    typeTwoKeys('alt', 'f')
    # jump to attribute
    for _ in range(attributeNumber):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type Enter to commit change
    typeEnter()

#################################################
    # delete
#################################################


def deleteMode():
    """
    @brief Change to delete mode
    """
    typeKey('d')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def deleteUsingSuprKey():
    """
    @brief delete using SUPR key
    """
    typeKey('del')
    # wait for GL Debug
    time.sleep(DELAY_REMOVESELECTION)


def changeRemoveOnlyGeometryPoint(referencePosition):
    """
    @brief Enable or disable 'Remove only geometry point'
    """
    # select delete mode again to set mode
    deleteMode()
    # jump to checkbox
    typeTab()
    # type SPACE to change value
    typeSpace()


def protectElements(referencePosition):
    """
    @brief Protect or unprotect delete elements
    """
    # select delete mode again to set mode
    deleteMode()
    # jump to checkbox
    for _ in range(4):
        typeTab()
    # type SPACE to change value
    typeSpace()


def waitDeleteWarning():
    """
    @brief close warning about automatically delete additionals
    """
    # wait 0.5 second to question dialog
    time.sleep(DELAY_QUESTION)
    # press enter to close dialog
    typeEnter()

#################################################
    # select mode
#################################################


def selectMode():
    """
    @brief Change to select mode
    """
    typeKey('s')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def abortSelection():
    """
    @brief abort current selection
    """
    # type ESC to abort current selection
    typeEscape()


def lockSelection(glType):
    """
    @brief lock selection by glType
    """
    # focus current frame
    focusOnFrame()
    # move mouse
    pyautogui.moveTo(550, 200)
    # open Lock menu
    typeTwoKeys('alt', 'o')
    # go to selected glType
    for _ in range(glType):
        typeKey("down")
    # type enter to save change
    typeSpace()


def selectDefault():
    """
    @brief select elements with default frame values
    """
    # focus current frame
    focusOnFrame()
    for _ in range(15):
        typeTab()
    # type enter to select it
    typeEnter()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def saveSelection():
    """
    @brief save selection
    """
    focusOnFrame()
    # jump to save
    for _ in range(22):
        typeTab()
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    filename = os.path.join(_TEXTTEST_SANDBOX, "selection.txt")
    pasteIntoTextField(filename)
    typeEnter()


def loadSelection():
    """
    @brief save selection
    """
    focusOnFrame()
    # jump to save
    for _ in range(25):
        typeTab()
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    filename = os.path.join(_TEXTTEST_SANDBOX, "selection.txt")
    pasteIntoTextField(filename)
    typeEnter()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectItems(elementClass, elementType, attribute, value):
    """
    @brief select items
    """
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for _ in range(8):
        typeTab()
    # paste the new elementClass
    pasteIntoTextField(elementClass)
    # jump to element
    for _ in range(3):
        typeTab()
    # paste the new elementType
    pasteIntoTextField(elementType)
    # jump to attribute
    for _ in range(2):
        typeTab()
    # paste the new attribute
    pasteIntoTextField(attribute)
    # jump to value
    for _ in range(2):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to select it
    typeEnter()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def deleteSelectedItems():
    """
    @brief delete selected items
    """
    typeKey('del')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def modificationModeAdd():
    """
    @brief set modification mode "add"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "add"
    for _ in range(3):
        typeTab()
    # select it
    typeSpace()


def modificationModeRemove():
    """
    @brief set modification mode "remove"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "remove"
    for _ in range(4):
        typeTab()
    # select it
    typeSpace()


def modificationModeKeep():
    """
    @brief set modification mode "keep"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "keep"
    for _ in range(5):
        typeTab()
    # select it
    typeSpace()


def modificationModeReplace():
    """
    @brief set modification mode "replace"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "replace"
    for _ in range(6):
        typeTab()
    # select it
    typeSpace()


def selectionRectangle(referencePosition, positionA, positionB):
    """
    @brief select using an rectangle
    """
    # Leave Shift key pressedX
    typeKeyDown('shift')
    # move element
    dragDrop(referencePosition, positionA.x, positionA.y, positionB.x, positionB.y)
    # wait after key up
    time.sleep(DELAY_KEY)
    # Release Shift key
    typeKeyUp('shift')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionClear():
    """
    @brief clear selection
    """
    # focus current frame
    focusOnFrame()
    for _ in range(21):
        typeTab()
    # type space to select clear option
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionInvert():
    """
    @brief invert selection
    """
    # focus current frame
    focusOnFrame()
    for _ in range(24):
        typeTab()
    # type space to select invert operation
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionInvertData():
    """
    @brief invert selection
    """
    # focus current frame
    focusOnFrame()
    for _ in range(27):
        typeTab()
    # type space to select invert operation
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


#################################################
    # traffic light
#################################################

def selectTLSMode():
    """
    @brief Change to traffic light mode
    """
    typeKey('t')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def createTLS():
    """
    @brief Create TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.create):
        typeTab()
    # create TLS
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def createTLSOverlapped(junction):
    """
    @brief Create TLS in overlapped junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.createOverlapped):
        typeTab()
    for _ in range(junction):
        typeSpace()
    for _ in range(attrs.TLS.createOverlapped):
        typeTab()
    # create TLS
    typeSpace()


def copyTLS(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.copyJoined):
            typeTab()
    else:
        for _ in range(attrs.TLS.copySingle):
            typeTab()
    # create TLS
    typeSpace()


def joinTSL():
    """
    @brief join TLS
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.joinTLS):
        typeTab()
    # create TLS
    typeSpace()


def disJoinTLS():
    """
    @brief disjoin the current TLS
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.disjoinTLS):
        typeTab()
    # create TLS
    typeSpace()


def deleteTLS(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to delete TLS button
    if (joined):
        for _ in range(attrs.TLS.deleteJoined):
            typeTab()
    else:
        for _ in range(attrs.TLS.deleteSingle):
            typeTab()
    # create TLS
    typeSpace()


def resetSingleTLSPhases(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.resetPhaseSingle):
            typeTab()
    else:
        for _ in range(attrs.TLS.resetPhaseJoined):
            typeTab()
    # create TLS
    typeSpace()


def resetAllTLSPhases(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.resetAllJoined):
            typeTab()
    else:
        for _ in range(attrs.TLS.resetAllSingle):
            typeTab()
    # create TLS
    typeSpace()


def pressTLSPhaseButton(position):
    """
    @brief add default phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def addDefaultPhase(position):
    """
    @brief add default phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def addDuplicatePhase(position):
    """
    @brief duplicate phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # move to button
    typeRight()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def addRedPhase(position):
    """
    @brief add red phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(2):
        typeRight()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def addYellowPhase(position):
    """
    @brief add yellow phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(3):
        typeRight()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def addGreenPhase(position):
    """
    @brief add green phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(4):
        typeRight()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)


def addGreenPriorityPhase(position):
    """
    @brief add priority phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeTab()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(5):
        typeRight()
    # add phase
    typeSpace()
    # wait
    time.sleep(DELAY_SELECT)

#################################################
    # shapes
#################################################


def shapeMode():
    """
    @brief change to shape mode
    """
    typeKey('p')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def createSquaredShape(referencePosition, position, size, close):
    """
    @brief Create squared Polygon in position with a certain size
    """
    # call create rectangled shape
    createRectangledShape(referencePosition, position, size, size, close)


def createRectangledShape(referencePosition, position, sizex, sizey, close):
    """
    @brief Create rectangle Polygon in position with a certain size
    """
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(referencePosition, position)
    leftClick(referencePosition, position, 0, (sizey / -2))
    leftClick(referencePosition, position, (sizex / -2), (sizey / -2))
    leftClick(referencePosition, position, (sizex / -2), 0)
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, position)
    # finish draw
    typeEnter()


def createLineShape(referencePosition, position, sizex, sizey, close):
    """
    @brief Create line Polygon in position with a certain size
    """
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(referencePosition, position)
    leftClick(referencePosition, position, (sizex / -2), (sizey / -2))
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, position)
    # finish draw
    typeEnter()


def changeColorUsingDialog(numTabs, color):
    """
    @brief modify default color using dialog
    """
    # focus current frame
    focusOnFrame()
    # go to length TextField
    for _ in range(numTabs):
        typeTab()
    typeSpace()
    # go to list of colors TextField
    for _ in range(2):
        typeInvertTab()
    # select color
    for _ in range(1 + color):
        typeKey('down')
    # go to accept button and press it
    typeTab()
    typeSpace()


def createGEOPOI():
    """
    @brief create GEO POI
    """
    # focus current frame
    focusOnFrame()
    # place cursor in create GEO POI
    for _ in range(20):
        typeTab()
    # create geoPOI
    typeSpace()


def GEOPOILonLat():
    """
    @brief change GEO POI format as Lon Lat
    """
    # focus current frame
    focusOnFrame()
    # place cursor in lon-lat
    for _ in range(16):
        typeTab()
    # Change current value
    typeSpace()


def GEOPOILatLon():
    """
    @brief change GEO POI format as Lat Lon
    """
    # focus current frame
    focusOnFrame()
    # place cursor in lat-lon
    for _ in range(17):
        typeTab()
    # Change current value
    typeSpace()


#################################################
    # TAZs
#################################################

def TAZMode():
    """
    @brief change to TAZ mode
    """
    typeKey('z')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


#################################################
    # datas
#################################################


def edgeData():
    """
    @brief change to edgeData mode
    """
    typeKey('e')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def edgeRelData():
    """
    @brief change to edgeRelData mode
    """
    typeKey('r')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def TAZRelData():
    """
    @brief change to TAZRelData mode
    """
    typeKey('z')
    # wait for gl debug
    time.sleep(DELAY_CHANGEMODE)


def createDataSet(dataSetID="newDataSet"):
    """
    @brief create dataSet
    """
    # focus current frame
    focusOnFrame()
    # go to create new dataSet
    for _ in range(2):
        typeTab()
    # enable create dataSet
    typeSpace()
    # go to create new dataSet
    typeTab()
    # create new ID
    pasteIntoTextField(dataSetID)
    # go to create new dataSet button
    typeTab()
    # create dataSet
    typeSpace()


def createDataInterval(begin="0", end="3600"):
    """
    @brief create dataInterval
    """
    # focus current frame
    focusOnFrame()
    # go to create new dataInterval
    for _ in range(5):
        typeTab()
    typeTab()
    # enable create dataInterval
    typeSpace()
    # go to create new dataInterval begin
    typeTab()
    # set begin
    pasteIntoTextField(begin)
    # go to end
    typeTab()
    # set end
    pasteIntoTextField(end)
    # go to create new dataSet button
    typeTab()
    # create dataSet
    typeSpace()

#################################################
    # Contextual menu
#################################################


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
    typeSpace()
    # check if go to submenu A
    if contextualMenuOperation.subMenuAPosition > 0:
        # place cursor over second operation
        for _ in range(contextualMenuOperation.subMenuAPosition):
            # wait before every down
            time.sleep(DELAY_KEY_TAB)
            # type down keys
            pyautogui.hotkey('down')
        # type space for select
        typeSpace()
        # check if go to submenu B
        if contextualMenuOperation.subMenuBPosition > 0:
            # place cursor over second operation
            for _ in range(contextualMenuOperation.subMenuBPosition):
                # wait before every down
                time.sleep(DELAY_KEY_TAB)
                # type down keys
                pyautogui.hotkey('down')
            # type space for select
            typeSpace()
