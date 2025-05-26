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
DELAY_QUESTION = 2
DELAY_SAVING = 1
DELAY_RELOAD = 3
DELAY_QUIT_NETEDIT = 5
DELAY_UNDOREDO = 1
DELAY_SELECT = 1
DELAY_RECOMPUTE = 3
DELAY_RECOMPUTE_VOLATILE = 5
DELAY_REMOVESELECTION = 2
DELAY_REFERENCE = 15

_NETEDIT_APP = os.environ.get("NETEDIT_BINARY", "netedit")
_TEXTTEST_SANDBOX = os.environ.get("TEXTTEST_SANDBOX", os.getcwd())
_REFERENCE_PNG = os.path.join(os.path.dirname(__file__), "reference.png")

#################################################
# Exclusive from pyautogui tests
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
    keyRelease("shift")
    keyRelease("control")
    keyRelease("alt")
    # Wait for Netedit reference
    return neteditProcess, getReferenceMatch(neteditProcess, makeScrenshot)

#################################################
    # keyboard functions
#################################################


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
    tr = {}
    if layout == "de":
        en = r"""y[];'\z/Y{}:"|Z<>?@#^&*()-_=+§"""
        de = u"""zü+öä#y-ZÜ*ÖÄ'Y;:_"§&/()=ß?´`^"""
        # join as keys and values
        tr.update(dict(zip(en, de)))
    return "".join(map(lambda x: tr.get(x, x), value))


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

#################################################
    # mouse functions
#################################################


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
        typeKey('down')
    typeKey('space')
    print("TestFunctions: Clicked over position",
          clickedPosition[0], '-', clickedPosition[1], "under element", underElement)

#################################################
    # processing functions
#################################################


def computeJunctions():
    """
    @brief compute junctions (rebuild network)
    """
    typeKey('F5')
    # wait for output
    time.sleep(DELAY_RECOMPUTE)


def computeJunctionsVolatileOptions(question=True):
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
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("loadViewport.xml")
    typeKey('enter')
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
    typeKey('tab')
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("viewport.xml")
    typeKey('enter')
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
        typeKey('tab')
    # Paste X
    if (len(zoom) > 0):
        updateText(zoom)
    # go to Y
    typeKey('tab')
    # Paste X
    if (len(x) > 0):
        updateText(x)
    # go to Y
    typeKey('tab')
    # Paste Y
    if (len(y) > 0):
        updateText(y)
    # go to Z
    typeKey('tab')
    # Paste Z
    if (len(z) > 0):
        updateText(z)
    # go to rotation
    typeKey('tab')
    # Paste rotation
    if (len(r) > 0):
        updateText(r)
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


def waitQuestion(answer):
    """
    @brief wait question of Netedit and select a yes/no answer (by default yes)
    """
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    if (answer == 'n'):
        typeKey('tab')
    typeKey('space')


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
        errorScreenshot.save("errorScreenshot.png")
        # kill netedit
        NeteditProcess.kill()
        print("TestFunctions: Error closing Netedit")
        # all keys up
        keyRelease("shift")
        keyRelease("control")
        keyRelease("alt")
        # exit
        return


def loadNetwork(useShortcut, waitTime=2):
    """
    @brief load network using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'o')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.loadNetwork):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("net.net.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("netAs.net.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadAdditionalElements(useShortcut, waitTime=2):
    """
    @brief load additional using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'a')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.aditionalElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.aditionalElements.load):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additionals.add.xml")
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additionalsAs.add.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadDemandElements(useShortcut, waitTime=2):
    """
    @brief load demand elements using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'd')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.demandElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.demandElements.load):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("routes.rou.xml")
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("routesAs.rou.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadDataElements(useShortcut, waitTime=2):
    """
    @brief load data elements using dialog
    """
    if (useShortcut):
        typeTwoKeys('ctrl', 'b')
    else:
        typeTwoKeys('alt', 'f')
        for _ in range(attrs.toolbar.file.dataElements.menu):
            typeKey('down')
        typeKey('space')
        for _ in range(attrs.toolbar.file.dataElements.load):
            typeKey('down')
        typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datas.dat.xml")
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datasAs.dat.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def loadMeanDataElements(waitTime=2):
    """
    @brief load mean data elements using dialog
    """
    # open load mean data dialog (because doesn't have shortcut)
    typeTwoKeys('alt', 'f')
    for _ in range(attrs.toolbar.file.meanDataElements.menu):
        typeKey('down')
    typeKey('space')
    for _ in range(attrs.toolbar.file.meanDataElements.load):
        typeKey('down')
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    # wait for saving
    time.sleep(waitTime)
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datas.med.add.xml")
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("datasAs.med.add.xml")
    typeKey('enter')
    # wait for saving
    time.sleep(waitTime)


def overwritte(value):
    """
    @brief check if overwritte loaded elements
    """
    if value == "yes":
        typeKey('space')
    elif value == "no":
        typeKey('tab')
        typeKey('space')
    else:
        typeKey('tab')
        typeKey('tab')
        typeKey('space')


def fixDemandElements(solution):
    """
    @brief fix stoppingPlaces
    """
    # select bullet depending of solution
    if (solution == "saveInvalids"):
        for _ in range(3):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(3):
            typeKey('tab')
        typeKey('space')
    elif (solution == "fixPositions"):
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        for _ in range(2):
            typeKey('tab')
        typeKey('space')
    elif (solution == "selectInvalids"):
        typeTwoKeys('shift', 'tab')
        typeKey('space')
        # go back and press accept
        typeKey('tab')
        typeKey('space')
    elif (solution == "activateFriendlyPos"):
        # default option, then press accept
        typeKey('space')
    else:
        # press cancel
        typeKey('tab')
        typeKey('space')


def openAboutDialog(waitingTime=DELAY_QUESTION):
    """
    @brief open and close about dialog
    """
    # type F12 to open about dialog
    typeKey('F12')
    # wait before closing
    time.sleep(waitingTime)
    # press enter to close dialog (Ok must be focused)
    typeKey('space')


def openNeteditConfigShortcut(waitTime=2):
    """
    @brief open configuration using shortcut
    """
    # open configuration dialog
    typeTwoKeys('ctrl', 'e')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("netedit_open.netecfg")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)


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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("saveConfigAs.netecfg")
    typeKey('enter')
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.net.xml")
    typeKey('enter')
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.netccfg")
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("net")
    typeKey('enter')
    # wait for loading
    time.sleep(waitTime)

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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.netecfg")
    typeKey('enter')
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.sumocfg")
    typeKey('enter')
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
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("config.sumocfg")
    typeKey('enter')
    # wait for saving
    time.sleep(DELAY_SAVING)

#################################################
    # change modes
#################################################


def changeSupermode(supermode):
    """
    @brief change supermode
    """
    match supermode:
        # common modes
        case "network":
            typeKey('F2')
        case "demand":
            typeKey('F3')
        case "data":
            typeKey('F4')
        case _:
            raise Exception("Invalid supermode")
    # wait for recompute
    time.sleep(DELAY_RECOMPUTE)


def changeMode(mode):
    """
    @brief change edit mode
    """
    match mode:
        # common modes
        case "inspect":
            typeKey('i')
        case "delete":
            typeKey('d')
        case "select":
            typeKey('s')
        # network modes
        case "createEdge":
            typeKey('e')
        case "move":
            typeKey('m')
        case "crossing":
            typeKey('r')
        case "connection":
            typeKey('c')
        case "additional":
            typeKey('a')
        case "TLS":
            typeKey('t')
        case "TAZ":
            typeKey('z')
        # demand modes
        case "route":
            typeKey('r')
        case "vehicle":
            typeKey('v')
        case "type":
            typeKey('t')
        case "person":
            typeKey('p')
        case "personPlan":
            typeKey('l')
        case "container":
            typeKey('c')
        case "containerPlan":
            typeKey('h')
        case "stop":
            typeKey('a')
        # data modes
        case "edgeData":
            typeKey('e')
        case "edgeRelData":
            typeKey('r')
        case "TAZRelData":
            typeKey('z')
        case "meanData":
            typeKey('m')
        case _:
            raise Exception("Invalid mode")
    # wait 1 second
    time.sleep(1)


def changeEditMode(key):
    """
    @brief Change edit mode (alt+1-9)
    """
    typeTwoKeys('alt', key)

#################################################
    # create edge mode
#################################################


def cancelEdge():
    """
    @brief Cancel current created edge (used in chain mode)
    """
    # type ESC to cancel current edge
    typeKey('esc')

#################################################
    # Inspect mode
#################################################


def modifyAttribute(attributeIndex, value, overlapped):
    """
    @brief modify attribute of type int/float/string
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    # paste the new value
    updateText(value)
    # type Enter to commit change
    typeKey('enter')


def modifyBoolAttribute(attributeIndex, overlapped):
    """
    @brief modify boolean attribute
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    # type SPACE to change value
    typeKey('space')


def modifyColorAttribute(attributeIndex, color, overlapped):
    """
    @brief modify color using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    typeKey('space')
    # go to list of colors TextField
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    # select color
    for _ in range(1 + color):
        typeKey('down')
    # go to accept button and press it
    typeKey('tab')
    typeKey('space')


def modifyAttributeVClassDialog(attribute, vClass, overlapped, disallowAll=True, cancel=False, reset=False):
    """
    @brief modify vclass attribute using dialog
    """
    # open dialog
    modifyBoolAttribute(attribute, overlapped)
    # first check if disallow all
    if (disallowAll):
        for _ in range(attrs.dialog.allowVClass.disallowAll):
            typeKey('tab')
        typeKey('space')
        # go to vClass
        for _ in range(vClass - attrs.dialog.allowVClass.disallowAll):
            typeKey('tab')
        # Change current value
        typeKey('space')
    else:
        # go to vClass
        for _ in range(vClass):
            typeKey('tab')
        # Change current value
        typeKey('space')
    # check if cancel
    if (cancel):
        for _ in range(attrs.dialog.allowVClass.cancel - vClass):
            typeKey('tab')
        typeKey('space')
    elif (reset):
        for _ in range(attrs.dialog.allowVClass.reset - vClass):
            typeKey('tab')
        typeKey('space')
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    else:
        for _ in range(attrs.dialog.allowVClass.accept - vClass):
            typeKey('tab')
        typeKey('space')


def modifyAdditionalFileDialog(attributeIndex, overlapped, waitTime=2):
    """
    @brief modify default additional file using dialog
    """
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if overlapped:
        for _ in range(attributeIndex + 1 + attrs.editElements.overlapped):
            typeKey('tab')
    else:
        for _ in range(attributeIndex + 1):
            typeKey('tab')
    # Change current value
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additional.secondFile.add.xml")
    typeKey('enter')


def modifyAdditionalFile(attributeIndex, overlapped):
    """
    @brief modify default additional file
    """
    modifyAttribute(attributeIndex, _TEXTTEST_SANDBOX + "/additional.thirdFile.add.xml", overlapped)


def checkUndoRedo(referencePosition, offsetX=0, offsetY=0):
    """
    @brief Check undo-redo
    """
    # Check undo
    undo(referencePosition, 9, offsetX)
    # Check redo
    redo(referencePosition, 9, offsetY)


def checkParameters(referencePosition, attributeIndex, overlapped, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeIndex, "dummyGenericParameters", overlapped)
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeIndex, "key1|key2|key3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "key1=value1|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeIndex, "key1=|key2=|key3=", overlapped)
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeIndex, "", overlapped)
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeIndex, "key1duplicated=value1|key1duplicated=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", overlapped)
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeIndex, "keyInvalid.;%>%$$=value1|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=value2|key3=value3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "keyFinal1=value1|keyFinal2=value2|keyFinal3=value3", overlapped)
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)


def checkDoubleParameters(referencePosition, attributeIndex, overlapped, offsetX=0, offsetY=0):
    """
    @brief Check generic parameters
    """
    # Change generic parameters with an invalid value (dummy)
    modifyAttribute(attributeIndex, "dummyGenericParameters", overlapped)
    # Change generic parameters with an invalid value (invalid format)
    modifyAttribute(attributeIndex, "key1|key2|key3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "key1=1|key2=2|key3=3", overlapped)
    # Change generic parameters with a valid value (empty values)
    modifyAttribute(attributeIndex, "key1=|key2=|key3=", overlapped)
    # Change generic parameters with a valid value (clear parameters)
    modifyAttribute(attributeIndex, "", overlapped)
    # Change generic parameters with an valid value (duplicated keys)
    modifyAttribute(attributeIndex, "key1duplicated=1|key1duplicated=2|key3=3", overlapped)
    # Change generic parameters with a valid value (duplicated values)
    modifyAttribute(attributeIndex, "key1=valueDuplicated|key2=valueDuplicated|key3=valueDuplicated", overlapped)
    # Change generic parameters with an invalid value (invalid key characters)
    modifyAttribute(attributeIndex, "keyInvalid.;%>%$$=1|key2=2|key3=3", overlapped)
    # Change generic parameters with a invalid value (invalid value characters)
    modifyAttribute(attributeIndex, "key1=valueInvalid%;%$<>$$%|key2=2|key3=3", overlapped)
    # Change generic parameters with a valid value
    modifyAttribute(attributeIndex, "keyFinal1=1|keyFinal2=2|keyFinal3=3", overlapped)
    # Check undoRedo
    checkUndoRedo(referencePosition, offsetX, offsetY)

#################################################
    # Move mode
#################################################


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


def moveGeometryPoint(referencePosition, originalPosition, destinyPositionA, destinyPositionB):
    """
    @brief move geometry point
    """
    leftClick(referencePosition, originalPosition)
    # move element
    dragDrop(referencePosition, originalPosition.x, originalPosition.y, destinyPositionA.x, destinyPositionA.y)
    dragDrop(referencePosition, destinyPositionA.x, destinyPositionA.y, destinyPositionB.x, destinyPositionB.y)


def toggleMoveEntireShape():
    """
    @brief toggle move entire shape
    """
    # focus current frame
    focusOnFrame()
    for _ in range(attrs.move.moveWholePolygon):
        typeKey('tab')
    # type space to create crossing
    typeKey('space')

#################################################
    # crossings
#################################################


def createCrossing(hasTLS):
    """
    @brief create crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to create crossing button depending of hasTLS
    if hasTLS:
        for _ in range(attrs.crossing.createTLS.button):
            typeKey('tab')
    else:
        for _ in range(attrs.crossing.create.button):
            typeKey('tab')
    # type space to create crossing
    typeKey('space')


def modifyCrossingDefaultValue(numtabs, value):
    """
    @brief change default int/real/string crossing default value
    """
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + attrs.crossing.firstField):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def modifyCrossingDefaultBoolValue(numtabs):
    """
    @brief change default boolean crossing default value
    """
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + attrs.crossing.firstField):
        typeKey('tab')
    # type space to change value
    typeKey('space')


def crossingClearEdges():
    """
    @brief clear crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to clear button
    for _ in range(attrs.crossing.clearEdges):
        typeKey('tab')
    # type space to activate button
    typeKey('space')


def crossingInvertEdges():
    """
    @brief invert crossing
    """
    # focus current frame
    focusOnFrame()
    # jump to invert button
    for _ in range(attrs.crossing.invertEdges):
        typeKey('tab')
    # type space to activate button
    typeKey('space')

#################################################
    # Connection mode
#################################################


def createConnection(referencePosition, fromLanePosition, toLanePosition, mode=""):
    """
    @brief create connection
    """
    # check if connection has to be created in certain mode
    if mode == "conflict":
        keyPress('ctrl')
    elif mode == "yield":
        keyPress('shift')
    # select first lane
    leftClick(referencePosition, fromLanePosition)
    # select another lane for create a connection
    leftClick(referencePosition, toLanePosition)
    # check if connection has to be created in certain mode
    if mode == "conflict":
        keyRelease('ctrl')
    elif mode == "yield":
        keyRelease('shift')


def saveConnectionEdit():
    """
    @brief Change to crossing mode
    """
    # focus current frame
    focusOnFrame()
    # go to cancel button
    for _ in range(attrs.connection.saveConnections):
        typeKey('tab')
    # type space to press button
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)

#################################################
    # additionals
#################################################


def changeElement(element):
    """
    @brief change element (Additional, shape, vehicle...)
    """
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    for _ in range(attrs.additionals.changeElement):
        typeKey('tab')
    # paste the new value
    updateText(element)
    # type enter to save change
    typeKey('enter')


def changeDefaultValue(attributeIndex, value):
    """
    @brief modify default int/double/string value of an additional, shape, vehicle...
    """
    # focus current frame
    focusOnFrame()
    # go to value TextField
    for _ in range(attributeIndex):
        typeKey('tab')
    # paste new value
    updateText(value)
    # type enter to save new value
    typeKey('enter')


def changeDefaultBoolValue(attributeIndex):
    """
    @brief modify default bool value of an additional, shape, vehicle...
    """
    # focus current frame
    focusOnFrame()
    # place cursor in check Box position
    for _ in range(attributeIndex):
        typeKey('tab')
    # Change current value
    typeKey('space')


def changeDefaultAllowDisallowValue(attributeIndex):
    """
    @brief modify allow/disallow values
    """
    # open dialog
    changeDefaultBoolValue(attributeIndex)
    # select vtypes
    for _ in range(2):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # select vtypes
    for _ in range(6):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # select vtypes
    for _ in range(12):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # select vtypes
    for _ in range(11):
        typeKey('tab')
    # Change current value
    typeKey('space')


def changeAdditionalFileDialog(attributeIndex, waitTime=2):
    """
    @brief modify default additional file using dialog
    """
    # focus current frame
    focusOnFrame()
    for _ in range(attributeIndex):
        typeKey('tab')
    # Change current value
    typeKey('space')
    # wait for saving
    time.sleep(waitTime)
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    updateText(_TEXTTEST_SANDBOX)
    typeKey('enter')
    updateText("additional.secondFile.add.xml")
    typeKey('enter')


def changeAdditionalFile(attributeIndex):
    """
    @brief modify default additional file
    """
    changeDefaultValue(attributeIndex, _TEXTTEST_SANDBOX + "/additional.thirdFile.add.xml")


def selectAdditionalChild(attributeIndex, childNumber):
    """
    @brief select child of additional
    """
    # focus current frame
    focusOnFrame()
    # place cursor in the list of childs
    for _ in range(attributeIndex + 1):
        typeKey('tab')
    # select child
    for _ in range(childNumber):
        typeKey('down')
    typeKey('space')
    # use TAB to select additional child
    typeKey('tab')


def fixStoppingPlace(solution):
    """
    @brief fix stoppingPlaces
    """
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)
    # select bullet depending of solution
    if (solution == "saveInvalids"):
        for _ in range(3):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        for _ in range(3):
            typeKey('tab')
        typeKey('space')
    elif (solution == "fixPositions"):
        for _ in range(2):
            typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        for _ in range(2):
            typeKey('tab')
        typeKey('space')
    elif (solution == "selectInvalids"):
        typeTwoKeys('shift', 'tab')
        typeKey('space')
    # go back and press accept
        typeKey('tab')
        typeKey('space')
    elif (solution == "activateFriendlyPos"):
        # default option, then press accept
        typeKey('space')
    else:
        # press cancel
        typeKey('tab')
        typeKey('space')

#################################################
    # demand elements
#################################################


def changeRouteMode(value):
    """
    @brief change route mode
    """
    # focus current frame
    focusOnFrame()
    # jump to route mode
    for _ in range(2):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def changeRouteVClass(value):
    """
    @brief change vClass mode
    """
    # focus current frame
    focusOnFrame()
    # jump to vClass
    for _ in range(4):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def fixDemandElement(value):
    """
    @brief fix demand element
    """
    # focus current frame
    focusOnFrame()
    # jump to option
    for _ in range(value):
        typeTwoKeys('shift', 'tab')
    # type space to select
    typeKey('space')
    # accept
    typeTwoKeys('alt', 'a')

#################################################
    # person elements
#################################################


def changePersonMode(value):
    """
    @brief change person mode
    """
    # focus current frame
    focusOnFrame()
    # jump to person mode
    typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def changePersonVClass(value):
    """
    @brief change vClass mode
    """
    # focus current frame
    focusOnFrame()
    # jump to vClass
    for _ in range(3):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def changePersonPlan(personPlan, flow):
    """
    @brief change personPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to person plan
    if (flow):
        for _ in range(29):
            typeKey('tab')
    else:
        for _ in range(22):
            typeKey('tab')
    # paste the new personPlan
    updateText(personPlan)
    # type enter to save change
    typeKey('enter')

#################################################
    # container elements
#################################################


def changeContainerMode(value):
    """
    @brief change container mode
    """
    # focus current frame
    focusOnFrame()
    # jump to container mode
    typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def changeContainerVClass(value):
    """
    @brief change vClass mode
    """
    # focus current frame
    focusOnFrame()
    # jump to vClass
    for _ in range(3):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to save change
    typeKey('enter')


def changeContainerPlan(containerPlan, flow):
    """
    @brief change containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to container plan
    if (flow):
        for _ in range(29):
            typeKey('tab')
    else:
        for _ in range(22):
            typeKey('tab')
    # paste the new containerPlan
    updateText(containerPlan)
    # type enter to save change
    typeKey('enter')

#################################################
    # personPlan elements
#################################################


def changePersonPlanMode(personPlan):
    """
    @brief change containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to person plan
    for _ in range(5):
        typeKey('tab')
    # paste the new containerPlan
    updateText(personPlan)
    # type enter to save change
    typeKey('enter')


def selectPerson(person):
    """
    @brief select person in containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to person plan
    for _ in range(2):
        typeKey('tab')
    # paste the new containerPlan
    updateText(person)
    # type enter to save change
    typeKey('enter')

#################################################
    # containerPlan elements
#################################################


def changeContainerPlanMode(containerPlan):
    """
    @brief change containerPlan
    """
    # focus current frame
    focusOnFrame()
    # jump to container plan
    for _ in range(5):
        typeKey('tab')
    # paste the new containerPlan
    updateText(containerPlan)
    # type enter to save change
    typeKey('enter')

#################################################
    # stop elements
#################################################


def changeStopParent(stopParent):
    """
    @brief change stop parent
    """
    # focus current frame
    focusOnFrame()
    for _ in range(2):
        typeKey('tab')
    # paste the new stop parent
    updateText(stopParent)
    # type enter to save change
    typeKey('enter')


def changeStopType(stopType):
    """
    @brief change stop type
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(5):
        typeKey('tab')
    # paste the new personPlan
    updateText(stopType)
    # type enter to save change
    typeKey('enter')

#################################################
    # vType elements
#################################################


def createVType():
    """
    @brief create vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.create):
        typeKey('tab')
    # type space
    typeKey('space')


def deleteVType():
    """
    @brief delete vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.delete):
        typeKey('tab')
    # type space
    typeKey('space')


def copyVType():
    """
    @brief copy vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.copy):
        typeKey('tab')
    # type space
    typeKey('space')


def openVTypeDialog():
    """
    @brief create vType
    """
    # focus current frame
    focusOnFrame()
    # jump to stop type
    for _ in range(attrs.type.buttons.dialog):
        typeKey('tab')
    # type space
    typeKey('space')
    # wait some second to question dialog
    time.sleep(DELAY_QUESTION)


def closeVTypeDialog():
    """
    @brief close vType dialog saving elements
    """
    typeTwoKeys('alt', 'a')


def modifyVTypeAttribute(attributeIndex, value):
    """
    @brief modify VType attribute of type int/float/string
    """
    # focus dialog
    typeTwoKeys('alt', 'f')
    # jump to attribute
    for _ in range(attributeIndex):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type Enter to commit change
    typeKey('enter')

#################################################
    # delete
#################################################


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
    changeMode("delete")
    # jump to checkbox
    typeKey('tab')
    # type SPACE to change value
    typeKey('space')


def protectElements(referencePosition):
    """
    @brief Protect or unprotect delete elements
    """
    # select delete mode again to set mode
    changeMode("delete")
    # jump to checkbox
    for _ in range(4):
        typeKey('tab')
    # type SPACE to change value
    typeKey('space')


def waitDeleteWarning():
    """
    @brief close warning about automatically delete additionals
    """
    # wait 0.5 second to question dialog
    time.sleep(DELAY_QUESTION)
    # press enter to close dialog
    typeKey('enter')

#################################################
    # select mode
#################################################


def abortSelection():
    """
    @brief abort current selection
    """
    # type ESC to abort current selection
    typeKey('esc')


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
    typeKey('space')


def selectDefault():
    """
    @brief select elements with default frame values
    """
    # focus current frame
    focusOnFrame()
    for _ in range(15):
        typeKey('tab')
    # type enter to select it
    typeKey('enter')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def saveSelection():
    """
    @brief save selection
    """
    focusOnFrame()
    # jump to save
    for _ in range(22):
        typeKey('tab')
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    filename = os.path.join(_TEXTTEST_SANDBOX, "selection.txt")
    updateText(filename)
    typeKey('enter')


def loadSelection():
    """
    @brief save selection
    """
    focusOnFrame()
    # jump to save
    for _ in range(25):
        typeKey('tab')
    typeKey('space')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    filename = os.path.join(_TEXTTEST_SANDBOX, "selection.txt")
    updateText(filename)
    typeKey('enter')
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
        typeKey('tab')
    # paste the new elementClass
    updateText(elementClass)
    # jump to element
    for _ in range(2):
        typeKey('tab')
    # paste the new elementType
    updateText(elementType)
    # jump to attribute
    for _ in range(3):
        typeKey('tab')
    # paste the new attribute
    updateText(attribute)
    # jump to value
    for _ in range(2):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to select it
    typeKey('enter')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectStoppingPlaceItems(elementClass, stoppingPlace, elementType, attribute, value):
    """
    @brief select items
    """
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for _ in range(8):
        typeKey('tab')
    # paste the new elementClass
    updateText(elementClass)
    # jump to element
    for _ in range(2):
        typeKey('tab')
    # paste the new elementType
    updateText(stoppingPlace)
    # jump to stoppingPlace
    for _ in range(2):
        typeKey('tab')
    # paste the new elementType
    updateText(elementType)
    # jump to attribute
    for _ in range(3):
        typeKey('tab')
    # paste the new attribute
    updateText(attribute)
    # jump to value
    for _ in range(2):
        typeKey('tab')
    # paste the new value
    updateText(value)
    # type enter to select it
    typeKey('enter')
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
        typeKey('tab')
    # select it
    typeKey('space')


def modificationModeRemove():
    """
    @brief set modification mode "remove"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "remove"
    for _ in range(4):
        typeKey('tab')
    # select it
    typeKey('space')


def modificationModeKeep():
    """
    @brief set modification mode "keep"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "keep"
    for _ in range(5):
        typeKey('tab')
    # select it
    typeKey('space')


def modificationModeReplace():
    """
    @brief set modification mode "replace"
    """
    # focus current frame
    focusOnFrame()
    # jump to mode "replace"
    for _ in range(6):
        typeKey('tab')
    # select it
    typeKey('space')


def selectionRectangle(referencePosition, positionA, positionB):
    """
    @brief select using an rectangle
    """
    # Leave Shift key pressedX
    keyPress('shift')
    # move element
    dragDrop(referencePosition, positionA.x, positionA.y, positionB.x, positionB.y)
    # wait after key up
    time.sleep(DELAY_KEY)
    # Release Shift key
    keyRelease('shift')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionClear():
    """
    @brief clear selection
    """
    # focus current frame
    focusOnFrame()
    for _ in range(21):
        typeKey('tab')
    # type space to select clear option
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionInvert():
    """
    @brief invert selection
    """
    # focus current frame
    focusOnFrame()
    for _ in range(24):
        typeKey('tab')
    # type space to select invert operation
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


def selectionInvertData():
    """
    @brief invert selection
    """
    # focus current frame
    focusOnFrame()
    for _ in range(27):
        typeKey('tab')
    # type space to select invert operation
    typeKey('space')
    # wait for gl debug
    time.sleep(DELAY_SELECT)

#################################################
    # traffic light
#################################################


def createTLS():
    """
    @brief Create TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.create):
        typeKey('tab')
    # create TLS
    typeKey('space')
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
        typeKey('tab')
    for _ in range(junction):
        typeKey('space')
    for _ in range(attrs.TLS.createOverlapped):
        typeKey('tab')
    # create TLS
    typeKey('space')


def copyTLS(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.copyJoined):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.copySingle):
            typeKey('tab')
    # create TLS
    typeKey('space')


def joinTSL():
    """
    @brief join TLS
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.joinTLS):
        typeKey('tab')
    # create TLS
    typeKey('space')


def disJoinTLS():
    """
    @brief disjoin the current TLS
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(attrs.TLS.disjoinTLS):
        typeKey('tab')
    # create TLS
    typeKey('space')


def deleteTLS(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to delete TLS button
    if (joined):
        for _ in range(attrs.TLS.deleteJoined):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.deleteSingle):
            typeKey('tab')
    # create TLS
    typeKey('space')


def resetSingleTLSPhases(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.resetPhaseSingle):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.resetPhaseJoined):
            typeKey('tab')
    # create TLS
    typeKey('space')


def resetAllTLSPhases(joined):
    """
    @brief copy TLS in the current selected Junction
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    if (joined):
        for _ in range(attrs.TLS.resetAllJoined):
            typeKey('tab')
    else:
        for _ in range(attrs.TLS.resetAllSingle):
            typeKey('tab')
    # create TLS
    typeKey('space')


def pressTLSPhaseButton(position):
    """
    @brief add default phase
    """
    # focus current frame
    focusOnFrame()
    # type tab 2 times to jump to create TLS button
    for _ in range(position):
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # add phase
    typeKey('space')
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
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # add phase
    typeKey('space')
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
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # move to button
    typeKey('right')
    # add phase
    typeKey('space')
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
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(2):
        typeKey('right')
    # add phase
    typeKey('space')
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
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(3):
        typeKey('right')
    # add phase
    typeKey('space')
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
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(4):
        typeKey('right')
    # add phase
    typeKey('space')
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
        typeKey('tab')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)
    # go to button
    for _ in range(5):
        typeKey('right')
    # add phase
    typeKey('space')
    # wait
    time.sleep(DELAY_SELECT)

#################################################
    # shapes
#################################################


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
    typeKey('enter')
    # create polygon
    leftClick(referencePosition, position)
    leftClick(referencePosition, position, 0, (sizey / -2))
    leftClick(referencePosition, position, (sizex / -2), (sizey / -2))
    leftClick(referencePosition, position, (sizex / -2), 0)
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, position)
    # finish draw
    typeKey('enter')


def createLineShape(referencePosition, position, sizex, sizey, close):
    """
    @brief Create line Polygon in position with a certain size
    """
    # focus current frame
    focusOnFrame()
    # start draw
    typeKey('enter')
    # create polygon
    leftClick(referencePosition, position)
    leftClick(referencePosition, position, (sizex / -2), (sizey / -2))
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, position)
    # finish draw
    typeKey('enter')


def changeColorUsingDialog(attributeIndex, color):
    """
    @brief modify default color using dialog
    """
    # focus current frame
    focusOnFrame()
    # go to length TextField
    for _ in range(attributeIndex):
        typeKey('tab')
    typeKey('space')
    # go to list of colors TextField
    for _ in range(2):
        typeTwoKeys('shift', 'tab')
    # select color
    for _ in range(1 + color):
        typeKey('down')
    # go to accept button and press it
    typeKey('tab')
    typeKey('space')


def createGEOPOI():
    """
    @brief create GEO POI
    """
    # focus current frame
    focusOnFrame()
    # place cursor in create GEO POI
    for _ in range(20):
        typeKey('tab')
    # create geoPOI
    typeKey('space')


def GEOPOILonLat():
    """
    @brief change GEO POI format as Lon Lat
    """
    # focus current frame
    focusOnFrame()
    # place cursor in lon-lat
    for _ in range(16):
        typeKey('tab')
    # Change current value
    typeKey('space')


def GEOPOILatLon():
    """
    @brief change GEO POI format as Lat Lon
    """
    # focus current frame
    focusOnFrame()
    # place cursor in lat-lon
    for _ in range(17):
        typeKey('tab')
    # Change current value
    typeKey('space')

#################################################
    # datas
#################################################


def createDataSet(dataSetID="newDataSet"):
    """
    @brief create dataSet
    """
    # focus current frame
    focusOnFrame()
    # go to create new dataSet
    for _ in range(2):
        typeKey('tab')
    # enable create dataSet
    typeKey('space')
    # go to create new dataSet
    typeKey('tab')
    # create new ID
    updateText(dataSetID)
    # go to create new dataSet button
    typeKey('tab')
    # create dataSet
    typeKey('space')


def createDataInterval(begin="0", end="3600"):
    """
    @brief create dataInterval
    """
    # focus current frame
    focusOnFrame()
    # go to create new dataInterval
    for _ in range(5):
        typeKey('tab')
    typeKey('tab')
    # enable create dataInterval
    typeKey('space')
    # go to create new dataInterval begin
    typeKey('tab')
    # set begin
    updateText(begin)
    # go to end
    typeKey('tab')
    # set end
    updateText(end)
    # go to create new dataSet button
    typeKey('tab')
    # create dataSet
    typeKey('space')


def createMeanData():
    """
    @brief create mean data
    """
    # focus current frame
    focusOnFrame()
    # go to create mean data
    for _ in range(5):
        typeKey('tab')
    # create mean data
    typeKey('space')


def deleteMeanData():
    """
    @brief delete mean data
    """
    # focus current frame
    focusOnFrame()
    # go to delete mean data
    for _ in range(6):
        typeKey('tab')
    # delete mean data
    typeKey('space')


def copyMeanData():
    """
    @brief copy mean data
    """
    # focus current frame
    focusOnFrame()
    # go to copy mean data
    for _ in range(7):
        typeKey('tab')
    # copy mean data
    typeKey('space')


def changeMeanData(meanData):
    """
    @brief change mean data
    """
    # focus current frame
    focusOnFrame()
    # go to delete mean data
    for _ in range(2):
        typeKey('tab')
    # set mean data
    updateText(meanData)
    # delete mean data
    typeKey('tab')

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
