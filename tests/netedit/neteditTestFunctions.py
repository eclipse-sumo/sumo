#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    neteditTestFunctions.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# Import libraries
from __future__ import print_function
import os
import sys
import subprocess
import pyautogui
import time
import pyperclip

# define delay before every operation
DELAY_KEY = 0.2
DELAY_KEY_TAB = 0.01
DELAY_MOUSE = 0.1
DELAY_QUESTION = 0.1
DELAY_REFERENCE = 30
DELAY_QUIT = 3
DELAY_UNDOREDO = 0.1
DELAY_SELECT = 0.1
DELAY_RECOMPUTE = 2
DELAY_RECOMPUTE_VOLATILE = 2
DELAY_REMOVESELECTION = 0.1

NeteditApp = os.environ.get("NETEDIT_BINARY", "netedit")
textTestSandBox = os.environ.get("TEXTTEST_SANDBOX", ".")

#################################################
# interaction functions
#################################################

"""
@brief type escape key
"""


def typeEscape():
    # type ESC key
    typeKey('esc')


"""
@brief type enter key
"""


def typeEnter():
    # type enter key
    typeKey('enter')


"""
@brief type space key
"""


def typeSpace():
    # type space key
    typeKey('space')


"""
@brief type tab key
"""


def typeTab():
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type keys
    pyautogui.hotkey('tab')


"""
@brief type Shift + Tab keys
"""


def typeInvertTab():
    # wait before every operation
    time.sleep(DELAY_KEY_TAB)
    # type two keys at the same time
    pyautogui.hotkey('shift', 'tab')


"""
@brief type single key
"""


def typeKey(key):
    # wait before every operation
    time.sleep(DELAY_KEY)
    # type keys
    pyautogui.hotkey(key)


"""
@brief type two keys at the same time (key1 -> key2)
"""


def typeTwoKeys(key1, key2):
    # wait before every operation
    time.sleep(DELAY_KEY)
    # type two keys at the same time
    pyautogui.hotkey(key1, key2)


"""
@brief type three keys at the same time (key1 -> key2 -> key3)
"""


def typeThreeKeys(key1, key2, key3):
    # wait before every operation
    time.sleep(DELAY_KEY)
    # type two keys at the same time
    pyautogui.hotkey(key1, key2, key3)


"""
@brief paste value into current text field
"""


def pasteIntoTextField(value, removePreviousContents=True):
    print(value)
    # remove previous content
    if(removePreviousContents):
        typeTwoKeys('ctrl', 'a')
        time.sleep(DELAY_KEY)
    # use copy & paste (due problems with certain characters, for example '|')
    pyperclip.copy(value)
    pyautogui.hotkey('ctrl', 'v')


"""
@brief do left click over a position relative to referencePosition (pink square)
"""


def leftClick(referencePosition, positionx, positiony):
    # wait before every operation
    time.sleep(DELAY_MOUSE)
    # obtain clicked position
    clickedPosition = [referencePosition[0] + positionx, referencePosition[1] + positiony]
    # click relative to offset
    pyautogui.click(clickedPosition)
    # wait after every operation
    time.sleep(DELAY_MOUSE)
    print("TestFunctions: Clicked over position", clickedPosition[0], '-', clickedPosition[1])


"""
@brief do left click over a position relative to referencePosition (pink square) while shift key is pressed
"""


def leftClickShift(referencePosition, positionx, positiony):
    # Leave Shift key pressed
    pyautogui.keyDown('shift')
    # wait before every operation
    time.sleep(DELAY_MOUSE)
    # obtain clicked position
    clickedPosition = [referencePosition[0] + positionx, referencePosition[1] + positiony]
    # click relative to offset
    pyautogui.click(clickedPosition)
    print("TestFunctions: Clicked with Shift key pressed over position", clickedPosition[0], '-', clickedPosition[1])
    # Release Shift key
    pyautogui.keyUp('shift')
    # wait after key up
    time.sleep(DELAY_KEY)


"""
@brief do left click over a position relative to referencePosition (pink square) while control key is pressed
"""


def leftClickControl(referencePosition, positionx, positiony):
    # Leave Shift key pressed
    pyautogui.keyDown('ctrl')
    # wait before every operation
    time.sleep(DELAY_MOUSE)
    # obtain clicked position
    clickedPosition = [referencePosition[0] + positionx, referencePosition[1] + positiony]
    # click relative to offset
    pyautogui.click(clickedPosition)
    print("TestFunctions: Clicked with Control key pressed over position", clickedPosition[0], '-', clickedPosition[1])
    # Release Shift key
    pyautogui.keyUp('ctrl')
    # wait after key up
    time.sleep(DELAY_KEY)


"""
@brief drag and drop from position 1 to position 2
"""


def dragDrop(referencePosition, x1, y1, x2, y2):
    # wait before every operation
    time.sleep(DELAY_KEY)
    # obtain from and to position
    fromPosition = [referencePosition[0] + x1, referencePosition[1] + y1]
    tromPosition = [referencePosition[0] + x2, referencePosition[1] + y2]
    # click relative to offset
    pyautogui.click(fromPosition)
    # drag mouse to X of 100, Y of 200 while holding down left mouse button
    pyautogui.dragTo(tromPosition[0], tromPosition[1], 1, button='left')


#################################################
# basic functions
#################################################


"""
@brief setup Netedit
"""


def setup(NeteditTests):
    # Open current environment file to obtain path to the Netedit App,
    # textTestSandBox
    envFile = os.path.join(NeteditTests, "currentEnvironment.tmp")
    if os.path.exists(envFile):
        global NeteditApp, textTestSandBox, currentOS
        with open(envFile) as env:
            NeteditApp, sandBox = [l.strip() for l in env.readlines()]
        if os.path.exists(sandBox):
            textTestSandBox = sandBox
        os.remove(envFile)


"""
@brief open Netedit
"""


def Popen(extraParameters, debugInformation):
    # set the default parameters of Netedit
    NeteditCall = [NeteditApp, '--gui-testing', '--window-pos', '50,50',
                   '--window-size', '700,500', '--no-warnings',
                   '--error-log', os.path.join(textTestSandBox, 'log.txt')]

    # check if debug output information has to be enabled
    if debugInformation:
        NeteditCall += ['--gui-testing-debug']

    # check if an existent net must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_net.net.xml")):
        NeteditCall += ['--sumo-net-file',
                        os.path.join(textTestSandBox, "input_net.net.xml")]

    # Check if additionals must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_additionals.add.xml")):
        NeteditCall += ['-a',
                        os.path.join(textTestSandBox, "input_additionals.add.xml")]

    # Check if demand elements must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_routes.rou.xml")):
        NeteditCall += ['-r',
                        os.path.join(textTestSandBox, "input_routes.rou.xml")]

    # check if a gui settings file has to be load
    if os.path.exists(os.path.join(textTestSandBox, "gui-settings.xml")):
        NeteditCall += ['--gui-settings-file',
                        os.path.join(textTestSandBox, "gui-settings.xml")]

    # set output for net
    NeteditCall += ['--output-file',
                    os.path.join(textTestSandBox, 'net.net.xml')]

    # set output for additionals
    NeteditCall += ['--additionals-output',
                    os.path.join(textTestSandBox, "additionals.xml")]

    # set output for demand elements
    NeteditCall += ['--demandelements-output',
                    os.path.join(textTestSandBox, "routes.xml")]

    # add extra parameters
    NeteditCall += extraParameters

    # return a subprocess with Netedit
    return subprocess.Popen(NeteditCall, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)


"""
@brief obtain reference referencePosition (pink square)
"""


def getReferenceMatch(neProcess, waitTime):
    # show information
    print("Finding reference")
    # capture screen and search reference
    positionOnScren = pyautogui.locateOnScreen('reference.png', waitTime)
    # check if pos was found
    if positionOnScren:
        # adjust position to center
        referencePosition = [positionOnScren[0] + 16, positionOnScren[1] + 16]
        # break loop
        print("TestFunctions: 'reference.png' found. Position: " +
              str(referencePosition[0]) + " - " + str(referencePosition[1]))
        # check that position is consistent (due scaling)
        if (referencePosition[0] != 304 or referencePosition[1] != 168):
            print("TestFunctions: Position of 'reference.png' isn't consistent. Check that interface scaling " +
                  "is 100% (See #3746)")
        return referencePosition
    else:
        # reference not found, then kill netedit process
        neProcess.kill()
        # print debug information
        sys.exit("TestFunctions: Killed Netedit process. 'reference.png' not found")


"""
@brief setup and start Netedit
"""


def setupAndStart(testRoot, extraParameters=[], debugInformation=True, waitTime=DELAY_REFERENCE):
    setup(testRoot)
    # Open Netedit
    NeteditProcess = Popen(extraParameters, debugInformation)
    # atexit.register(quit, NeteditProcess, False, False)
    # print debug information
    print("TestFunctions: Netedit opened successfully")
    # Wait for Netedit reference
    return NeteditProcess, getReferenceMatch(NeteditProcess, waitTime)


"""
@brief rebuild network
"""


def rebuildNetwork():
    typeKey('F5')
    # wait for output
    time.sleep(DELAY_RECOMPUTE)


"""
@brief rebuild network with volatile options
"""


def rebuildNetworkWithVolatileOptions(question=True):
    typeTwoKeys('shift', 'F5')
    # confirm recompute
    if question is True:
        waitQuestion('y')
        # wait for output
        time.sleep(DELAY_RECOMPUTE_VOLATILE)
    else:
        waitQuestion('n')


"""
@brief clean junction
"""


def cleanJunction():
    typeKey('F6')


"""
@brief join selected junctions
"""


def joinSelectedJunctions():
    typeKey('F7')


"""
@brief select focus on upper element of current frame
"""


def focusOnFrame():
    typeKey('F12')


"""
@brief undo last operation
"""


def undo(referencePosition, number):
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey('i')
    # click over referencePosition
    leftClick(referencePosition, 0, 0)
    for _ in range(number):
        typeTwoKeys('ctrl', 'z')
        time.sleep(DELAY_UNDOREDO)


"""
@brief undo last operation
"""


def redo(referencePosition, number):
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey('i')
    # click over referencePosition
    leftClick(referencePosition, 0, 0)
    for _ in range(number):
        typeTwoKeys('ctrl', 'y')
        time.sleep(DELAY_UNDOREDO)


"""
@brief set Zoom
"""


def setZoom(positionX, positionY, zoomLevel):
    # open edit viewport dialog
    typeKey('v')
    # by default is in "load" button, then go to position X
    for _ in range(3):
        typeTab()
    # Paste position X
    pasteIntoTextField(positionX)
    # go to Y
    typeTab()
    # Paste Position Y
    pasteIntoTextField(positionY)
    # go to Z
    typeTab()
    # Paste Zoom Z
    pasteIntoTextField(zoomLevel)
    # press OK Button using shortcut
    typeTwoKeys('alt', 'o')


"""
@brief wait question of Netedit and select a yes/no answer
"""


def waitQuestion(answer):
    # wait 0.5 second to question dialog
    time.sleep(DELAY_QUESTION)
    # Answer can be "y" or "n"
    typeTwoKeys('alt', answer)


"""
@brief quit Netedit quit
"""


def quit(NeteditProcess, openNetNonSavedDialog=False, saveNet=False,
         openAdditionalsNonSavedDialog=False, saveAdditionals=False,
         openShapesNonSavedDialog=False, saveShapes=False):
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
                waitQuestion('q')

        # Check if additionals must be saved
        if openAdditionalsNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveAdditionals:
                waitQuestion('s')
            else:
                waitQuestion('q')

        # Check if additionals must be saved
        if openShapesNonSavedDialog:
            # Wait some seconds
            time.sleep(DELAY_QUESTION)
            if saveShapes:
                waitQuestion('s')
            else:
                waitQuestion('q')

        # wait some seconds
        time.sleep(DELAY_QUIT)
        if NeteditProcess.poll() is not None:
            # print debug information
            print("TestFunctions: Netedit closed successfully")
        else:
            NeteditProcess.kill()
            # print debug information
            print("TestFunctions: Error closing Netedit")


"""
@brief load network as
"""


def openNetworkAs(waitTime=2):
    # open save network as dialog
    typeTwoKeys('ctrl', 'o')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(textTestSandBox)
    typeEnter()
    pasteIntoTextField("input_net_loadedmanually.net.xml")
    typeEnter()
    # wait for saving
    time.sleep(waitTime)


"""
@brief save network
"""


def saveNetwork():
    # save network using hotkey
    typeTwoKeys('ctrl', 's')
    # wait for debug
    time.sleep(DELAY_RECOMPUTE)


"""
@brief save network as
"""


def saveNetworkAs(waitTime=2):
    # open save network as dialog
    typeThreeKeys('ctrl', 'shift', 's')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(textTestSandBox)
    typeEnter()
    pasteIntoTextField("net.net.xml")
    typeEnter()
    # wait for saving
    time.sleep(waitTime)
    # wait for debug
    time.sleep(DELAY_RECOMPUTE)


"""
@brief save additionals
"""


def saveAdditionals():
    # save additionals using hotkey
    typeThreeKeys('ctrl', 'shift', 'd')


"""
@brief open and close about dialog
"""


def openAboutDialog(waitingTime=DELAY_QUESTION):
    # type F2 to open about dialog
    typeKey('F2')
    # wait before closing
    time.sleep(waitingTime)
    # press enter to close dialog (Ok must be focused)
    typeSpace()


"""
@brief open configuration using shortcut
"""


def openConfigurationShortcut(waitTime=2):
    # open configuration dialog
    typeThreeKeys('ctrl', 'shift', 'o')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(textTestSandBox)
    typeEnter()
    pasteIntoTextField("input_net.netccfg")
    typeEnter()
    # wait for loading
    time.sleep(waitTime)


"""
@brief save configuration using shortcut
"""


def savePlainXML(waitTime=2):
    # open configuration dialog
    typeTwoKeys('ctrl', 'l')
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    pasteIntoTextField(textTestSandBox)
    typeEnter()
    pasteIntoTextField("net")
    typeEnter()
    # wait for loading
    time.sleep(waitTime)

#################################################
# Create nodes and edges
#################################################


"""
@brief Change to create edge mode
"""


def createEdgeMode():
    typeKey('e')


"""
@brief Cancel current created edge (used in chain mode)
"""


def cancelEdge():
    # type ESC to cancel current edge
    typeEscape()


"""
@brief Change chain option
"""


def changeChainOption():
    # cancel current created edge
    cancelEdge()
    # jump to chain
    for _ in range(3):
        typeInvertTab()
    # change chain mode
    typeSpace()


"""
@brief Change two-way mode
"""


def changeTwoWayOption():
    # cancel current created edge
    cancelEdge()
    # jump to two way
    for _ in range(2):
        typeInvertTab()
    # change two way mode
    typeSpace()

#################################################
# Inspect mode
#################################################


"""
@brief go to inspect mode
"""


def inspectMode():
    typeKey('i')


"""
@brief modify attribute of type int/float/string
"""


def modifyAttribute(attributeNumber, value, overlapped):
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if (overlapped == True):
        for _ in range(attributeNumber + 5):
            typeTab()
    else:
        for _ in range(attributeNumber + 1):
            typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type Enter to commit change
    typeEnter()


"""
@brief modify boolean attribute
"""


def modifyBoolAttribute(attributeNumber, overlapped):
    # focus current frame
    focusOnFrame()
    # jump to attribute depending if it's a overlapped element
    if (overlapped == True):
        for _ in range(attributeNumber + 5):
            typeTab()
    else:
        for _ in range(attributeNumber + 1):
            typeTab()
    # type SPACE to change value
    typeSpace()

#################################################
# Move mode
#################################################


"""
@brief set move mode
"""


def moveMode():
    typeKey('m')


"""
@brief move element
"""


def moveElement(referencePosition, startX, startY, endX, endY):
    # move element
    dragDrop(referencePosition, startX, startY, endX, endY)

#################################################
# crossings
#################################################


"""
@brief Change to crossing mode
"""


def crossingMode():
    typeKey('r')


"""
@brief create crossing
"""


def createCrossing():
    # focus current frame
    focusOnFrame()
    # jump to create crossing button
    for _ in range(7):
        typeTab()
    # type space to create crossing
    typeSpace()


"""
@brief change default int/real/string crossing default value
"""


def modifyCrossingDefaultValue(numtabs, value):
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + 1):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()


"""
@brief change default boolean crossing default value
"""


def modifyCrossingDefaultBoolValue(numtabs):
    # focus current frame
    focusOnFrame()
    # jump to value
    for _ in range(numtabs + 1):
        typeTab()
    # type space to change value
    typeSpace()


"""
@brief clear crossing
"""


def crossingClearEdges(useSelectedEdges=False, thereIsSelectedEdges=False):
    # focus current frame
    focusOnFrame()
    if(useSelectedEdges and thereIsSelectedEdges):
        # jump to clear button
        for _ in range(1):
            typeTab()
    else:
        # jump to clear button
        for _ in range(1):
            typeTab()
    # type space to activate button
    typeSpace()


"""
@brief invert crossing
"""


def crossingInvertEdges(useSelectedEdges=False, thereIsSelectedEdges=False):
    # focus current frame
    focusOnFrame()
    if(useSelectedEdges and thereIsSelectedEdges):
        # jump to clear button
        for _ in range(1):
            typeTab()
    else:
        # jump to clear button
        for _ in range(2):
            typeTab()
    # type space to activate button
    typeSpace()


#################################################
# crossings
#################################################


"""
@brief Change to crossing mode
"""


def connectionMode():
    typeKey('c')


"""
@brief show connections (Note: Inspector mode has to be enabled)
"""


def toogleShowConnectionsInspectorMode():
    # focus current frame
    focusOnFrame()
    # go to check box
    typeInvertTab()
    # type space to toogle checkbox
    typeSpace()
    # focus frame again
    typeTab()


"""
@brief create connection
"""


def createConnection(referencePosition, fromLanePositionX, fromLanePositionY,
                     toLanePositionX, toLanePositionY, mode=""):
    # check if connection has to be created in certain mode
    if mode == "conflict":
        pyautogui.keyDown('ctrl')
    elif mode == "yield":
        pyautogui.keyDown('shift')
    # select first lane
    leftClick(referencePosition, fromLanePositionX, fromLanePositionY)
    # select another lane for create a connection
    leftClick(referencePosition, toLanePositionX, toLanePositionY)
    # check if connection has to be created in certain mode
    if mode == "conflict":
        pyautogui.keyUp('ctrl')
    elif mode == "yield":
        pyautogui.keyUp('shift')


"""
@brief Change to crossing mode
"""


def saveConnectionEdit():
    # focus current frame
    focusOnFrame()
    # go to cancel button
    for _ in range(2):
        typeTab()
    # type space to press button
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


#################################################
# additionals
#################################################


"""
@brief change to additional mode
"""


def additionalMode():
    typeKey('a')


"""
@brief change additional
"""


def changeAdditional(additional):
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    typeTab()
    # paste the new value
    pasteIntoTextField(additional)
    # type enter to save change
    typeEnter()


"""
@brief modify default int/double/string value of an additional
"""


def modifyAdditionalDefaultValue(numTabs, length):
    # focus current frame
    focusOnFrame()
    # go to length TextField
    for _ in range(numTabs + 1):
        typeTab()
    # paste new length
    pasteIntoTextField(length)
    # type enter to save new length
    typeEnter()


"""
@brief modify default boolean value of an additional
"""


def modifyAdditionalDefaultBoolValue(numTabs):
    # focus current frame
    focusOnFrame()
    # place cursor in check Box position
    for _ in range(numTabs + 1):
        typeTab()
    # Change current value
    typeSpace()


"""
@brief modify number of stopping place lines
"""


def modifyStoppingPlaceLines(numTabs, numLines):
    # focus current frame
    focusOnFrame()
    # go to add line
    for _ in range(numTabs + 1):
        typeTab()
    # add lines using space
    for _ in range(numLines):
        typeSpace()


"""
@brief fill lines to stopping places
"""


def fillStoppingPlaceLines(numTabs, numLines):
    # focus current frame
    focusOnFrame()
    # place cursor in the first line
    for _ in range(numTabs + 1):
        typeTab()
    # fill lines
    for x in range(numLines):
        # paste line and number
        pasteIntoTextField("Line" + str(x))
        # go to next field
        typeTab()


"""
@brief select child of additional
"""


def selectAdditionalChild(numTabs, childNumber):
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


"""
@brief fix stoppingPlaces
"""


def fixStoppingPlace(solution):
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
# delete
#################################################


"""
@brief Change to delete mode
"""


def deleteMode():
    typeKey('d')


"""
@brief delete using SUPR key
"""


def deleteUsingSuprKey():
    typeKey('del')
    # wait for GL Debug
    time.sleep(DELAY_REMOVESELECTION)


"""
@brief Enable or disable 'automatically delete Additionals'
"""


def changeAutomaticallyDeleteAdditionals(referencePosition):
    # select delete mode again to set mode
    deleteMode()
    # use TAB to go to check box
    typeTab()
    # type SPACE to change value
    typeSpace()


"""
@brief close warning about automatically delete additionals
"""


def waitAutomaticallyDeleteAdditionalsWarning():
    # wait 0.5 second to question dialog
    time.sleep(DELAY_QUESTION)
    # press enter to close dialog
    typeEnter()

#################################################
# select mode
#################################################


"""
@brief Change to select mode
"""


def selectMode():
    typeKey('s')


"""
@brief abort current selection
"""


def abortSelection():
    # type ESC to abort current selection
    typeEscape()


"""
@brief toogle select edges
"""


def toogleSelectEdges():
    focusOnFrame()
    # jump to toogle edge
    for _ in range(3):
        typeInvertTab()
    typeSpace()
    # Focus on frame again
    focusOnFrame()


"""
@brief toogle show connections (in select mode)
"""


def toogleShowConnections():
    focusOnFrame()
    # jump to toogle edge
    for _ in range(2):
        typeInvertTab()
    typeSpace()
    # Focus on frame again
    focusOnFrame()


"""
@brief lock selection by glType
"""


def lockSelection(glType):
    # focus current frame
    focusOnFrame()
    # go to selected glType
    for _ in range(glType):
        typeTab()
    # type enter to save change
    typeSpace()


"""
@brief select elements with default frame values
"""


def selectDefault():
    # focus current frame
    focusOnFrame()
    for _ in range(19):
        typeTab()
    # type enter to select it
    typeEnter()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


"""
@brief save selection
"""


def saveSelection():
    focusOnFrame()
    # jump to save
    for _ in range(24):
        typeTab()
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    filename = os.path.join(textTestSandBox, "selection.txt")
    pasteIntoTextField(filename)
    typeEnter()


"""
@brief save selection
"""


def loadSelection():
    focusOnFrame()
    # jump to save
    for _ in range(25):
        typeTab()
    typeSpace()
    # jump to filename TextField
    typeTwoKeys('alt', 'f')
    filename = os.path.join(textTestSandBox, "selection.txt")
    pasteIntoTextField(filename)
    typeEnter()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


"""
@brief select items
"""


def selectItems(elementClass, elementType, attribute, value):
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for _ in range(13):
        typeTab()
    # paste the new elementClass
    pasteIntoTextField(elementClass)
    # jump to element
    for _ in range(2):
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


"""
@brief delete selected items
"""


def deleteSelectedItems():
    typeKey('del')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


"""
@brief set modification mode "add"
"""


def modificationModeAdd():
    # focus current frame
    focusOnFrame()
    # jump to mode "add"
    for _ in range(9):
        typeTab()
    # select it
    typeSpace()


"""
@brief set modification mode "remove"
"""


def modificationModeRemove():
    # focus current frame
    focusOnFrame()
    # jump to mode "remove"
    for _ in range(10):
        typeTab()
    # select it
    typeSpace()


"""
@brief set modification mode "keep"
"""


def modificationModeKeep():
    # focus current frame
    focusOnFrame()
    # jump to mode "keep"
    for _ in range(11):
        typeTab()
    # select it
    typeSpace()


"""
@brief set modification mode "replace"
"""


def modificationModeReplace():
    # focus current frame
    focusOnFrame()
    # jump to mode "replace"
    for _ in range(12):
        typeTab()
    # select it
    typeSpace()


"""
@brief select using an rectangle
"""


def selectionRectangle(referencePosition, startX, startY, endX, endY):
    # Leave Shift key pressed
    pyautogui.keyDown('shift')
    # move element
    dragDrop(referencePosition, startX, startY, endX, endY)
    # Release Shift key
    pyautogui.keyUp('shift')
    # wait for gl debug
    time.sleep(DELAY_SELECT)


"""
@brief clear selection
"""


def selectionClear(previouslyInserted=False):
    # focus current frame
    focusOnFrame()
    for _ in range(22):
        typeTab()
    # type space to select clear option
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


"""
@brief invert selection
"""


def selectionInvert():
    # focus current frame
    focusOnFrame()
    for _ in range(23):
        typeTab()
    # type space to select invert operation
    typeSpace()
    # wait for gl debug
    time.sleep(DELAY_SELECT)


"""
@brief Toggle select edges and lanes
"""


def selectionToogleEdges():
    # focus current frame
    focusOnFrame()
    # go to check box "select edges"
    for _ in range(2):
        typeInvertTab()
    # type space to enable or disable edge selection
    typeSpace()

#################################################
# traffic light
#################################################


"""
@brief Change to traffic light mode
"""


def selectTLSMode():
    typeKey('t')


"""
@brief Create TLS in the current selected Junction
"""


def createTLS():
    # focus current frame
    focusOnFrame()
    # type tab 3 times to jump to create TLS button
    for _ in range(3):
        typeTab()
    # create TLS
    typeSpace()

#################################################
# shapes
#################################################


"""
@brief change to shape mode
"""


def shapeMode():
    typeKey('p')


"""
@brief change shape
"""


def changeShape(shape):
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    typeTab()
    # paste the new value
    pasteIntoTextField(shape)
    # type enter to save change
    typeEnter()


"""
@brief Create squared Polygon in position with a certain size
"""


def createSquaredPoly(referencePosition, positionx, positiony, size, close):
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(referencePosition, positionx, positiony)
    leftClick(referencePosition, positionx, positiony - (size / 2))
    leftClick(referencePosition, positionx - (size / 2), positiony - (size / 2))
    leftClick(referencePosition, positionx - (size / 2), positiony)
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, positionx, positiony)
    # finish draw
    typeEnter()


"""
@brief Create rectangle Polygon in position with a certain size
"""


def createRectangledPoly(referencePosition, positionx, positiony, sizex, sizey, close):
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(referencePosition, positionx, positiony)
    leftClick(referencePosition, positionx, positiony - (sizey / 2))
    leftClick(referencePosition, positionx - (sizex / 2), positiony - (sizey / 2))
    leftClick(referencePosition, positionx - (sizex / 2), positiony)
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, positionx, positiony)
    # finish draw
    typeEnter()


"""
@brief Create line Polygon in position with a certain size
"""


def createLinePoly(referencePosition, positionx, positiony, sizex, sizey, close):
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(referencePosition, positionx, positiony)
    leftClick(referencePosition, positionx - (sizex / 2), positiony - (sizey / 2))
    # check if polygon has to be closed
    if (close is True):
        leftClick(referencePosition, positionx, positiony)
    # finish draw
    typeEnter()


"""
@brief modify default int/double/string value of an shape
"""


def modifyShapeDefaultValue(numTabs, value):
    # focus current frame
    focusOnFrame()
    # go to length TextField
    for _ in range(numTabs + 1):
        typeTab()
    # paste new value
    pasteIntoTextField(value)
    # type enter to save new value
    typeEnter()


"""
@brief modify default color using dialog
"""


def changeColorUsingDialog(numTabs, color):
    # focus current frame
    focusOnFrame()
    # go to length TextField
    for _ in range(numTabs + 1):
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


"""
@brief modify default boolean value of an shape
"""


def modifyShapeDefaultBoolValue(numTabs):
    # focus current frame
    focusOnFrame()
    # place cursor in check Box position
    for _ in range(numTabs + 1):
        typeTab()
    # Change current value
    typeSpace()


"""
@brief create GEO POI
"""


def createGEOPOI():
    # focus current frame
    focusOnFrame()
    # place cursor in create GEO POI
    for _ in range(19):
        typeTab()
    # Change current value
    typeSpace()


"""
@brief change GEO POI format as Lon Lat
"""


def GEOPOILonLat():
    # focus current frame
    focusOnFrame()
    # place cursor in lon-lat
    for _ in range(15):
        typeTab()
    # Change current value
    typeSpace()


"""
@brief change GEO POI format as Lat Lon
"""


def GEOPOILatLon():
    # focus current frame
    focusOnFrame()
    # place cursor in lat-lon
    for _ in range(16):
        typeTab()
    # Change current value
    typeSpace()


#################################################
# Contextual menu
#################################################


def contextualMenuOperation(referencePosition, positionx, positiony, operation, suboperation1, suboperation2=0):
    # obtain clicked position
    clickedPosition = [referencePosition[0] + positionx, referencePosition[1] + positiony]
    # click relative to offset
    pyautogui.rightClick(clickedPosition)
    # place cursor over first operation
    for _ in range(operation):
        # wait before every down
        time.sleep(DELAY_KEY_TAB)
        # type down keys
        pyautogui.hotkey('down')
    if suboperation1 > 0:
        # type right key for the second menu
        typeSpace()
        # place cursor over second operation
        for _ in range(suboperation1):
            # wait before every down
            time.sleep(DELAY_KEY_TAB)
            # type down keys
            pyautogui.hotkey('down')
    if suboperation2 > 0:
        # type right key for the third menu
        typeSpace()
        # place cursor over third operation
        for _ in range(suboperation2):
            # wait before every down
            time.sleep(DELAY_KEY_TAB)
            # type down keys
            pyautogui.hotkey('down')
    # select current operation
    typeSpace()
