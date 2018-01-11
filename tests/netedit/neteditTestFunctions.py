#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
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
import os
import sys
import subprocess
import platform
import atexit
from sikuli import *

# define delay before every operation
DELAY = 0.2
DELAY_QUESTION = 1
DELAY_REFERENCE = 50
DELAY_QUIT = 3

Settings.MoveMouseDelay = 0.2
Settings.DelayBeforeDrop = 0.2
Settings.DelayAfterDrag = 0.2

neteditApp = os.environ.get("NETEDIT_BINARY", "netedit")
textTestSandBox = os.environ.get("TEXTTEST_SANDBOX", ".")
referenceImage = os.path.join("imageResources", "reference.png")

#################################################
# interaction functions
#################################################

""" 
@brief type escape key
"""


def typeEscape():
    # wait before every operation
    wait(DELAY)
    # type ESC key (Sikulix Function)
    type(Key.ESC)


"""
@brief type enter key
"""


def typeEnter():
    # wait before every operation
    wait(DELAY)
    # type enter key (Sikulix Function)
    type(Key.ENTER)


"""
@brief type space key
"""


def typeSpace():
    wait(DELAY)
    # type space key (Sikulix Function)
    type(Key.SPACE)


"""
@brief type tab key
"""


def typeTab():
    # wait before every operation
    wait(DELAY)
    # type tab key (Sikulix Function)
    type(Key.TAB)


"""
@brief type Shift + Tab keys
"""


def typeInvertTab():
    # wait before every operation
    wait(DELAY)
    # type Tab and Shift at the same time (Sikulix Function)
    type(Key.TAB, Key.SHIFT)


"""
@brief type single key
"""


def typeKey(key):
    # wait before every operation
    wait(DELAY)
    # type keys (Sikulix Function)
    type(key)


"""
@brief type two keys at the same time
"""


def typeTwoKeys(key1, key2):
    # wait before every operation
    wait(DELAY)
    # type two keys at the same time (Sikulix Function)
    type(key1, key2)


"""
@brief paste value into current text field
"""


def pasteIntoTextField(value, removePreviousContents=True):
    # wait before every operation
    wait(DELAY)
    # remove previous content
    if(removePreviousContents):
        typeTwoKeys("a", Key.CTRL)
        wait(0.1)
    # paste string (Sikulix Function)
    paste(value)


"""
@brief do left click over a position relative to match (pink square)
"""


def leftClick(match, positionx, positiony):
    # wait before every operation
    wait(DELAY)
    # click respect to offset
    click(match.getTarget().offset(positionx, positiony))


"""
@brief drag and drop from position 1 to position 2
"""


def dragDrop(match, x1, y1, x2, y2):
    # wait before every operation
    wait(DELAY)
    drag(match.getTarget().offset(x1, y1))
    wait(DELAY)
    dropAt(match.getTarget().offset(x2, y2))

#################################################
# basic functions
#################################################

"""
@brief setup netedit
"""


def setup(neteditTests):
    # Open current environment file to obtain path to the netedit app,
    # textTestSandBox
    envFile = os.path.join(neteditTests, "currentEnvironment.tmp")
    if os.path.exists(envFile):
        global neteditApp, textTestSandBox, currentOS
        with open(envFile) as env:
            neteditApp, sandBox = [l.strip() for l in env.readlines()]
        if os.path.exists(sandBox):
            textTestSandBox = sandBox
        os.remove(envFile)
    # get reference for match
    global referenceImage
    referenceImage = os.path.join(
        neteditTests, "imageResources", "reference.png")


"""
@brief open netedit
"""


def Popen(extraParameters, debugInformation):
    # set the default parameters of netedit
    neteditCall = [neteditApp, '--gui-testing', '--window-pos', '50,50',
                   '--window-size', '700,500', '--no-warnings',
                   '--error-log', os.path.join(textTestSandBox, 'log.txt')]

    # check if debug output information has to be enabled
    if debugInformation:
        neteditCall += ['--gui-testing-debug']

    # check if an existent net must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_net.net.xml")):
        neteditCall += ['--sumo-net-file',
                        os.path.join(textTestSandBox, "input_net.net.xml")]

    # Check if additionals must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_additionals.add.xml")):
        neteditCall += ['--sumo-additionals-file',
                        os.path.join(textTestSandBox, "input_additionals.add.xml")]

    # Check if shapes must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_shapes.add.xml")):
        neteditCall += ['--sumo-shapes-file',
                        os.path.join(textTestSandBox, "input_shapes.add.xml")]

    # check if a gui settings file has to be load
    if os.path.exists(os.path.join(textTestSandBox, "gui-settings.xml")):
        neteditCall += ['--gui-settings-file',
                        os.path.join(textTestSandBox, "gui-settings.xml")]

    # set output for net
    neteditCall += ['--output-file',
                    os.path.join(textTestSandBox, 'net.net.xml')]

    # set output for additionals
    neteditCall += ['--additionals-output',
                    os.path.join(textTestSandBox, "additionals.xml")]

    # set output for shapes
    neteditCall += ['--shapes-output',
                    os.path.join(textTestSandBox, "shapes.xml")]

    # add extra parameters
    neteditCall += extraParameters

    # return a subprocess with netedit
    return subprocess.Popen(neteditCall, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)


"""
@brief obtain reference match (pink square)
"""


def getReferenceMatch(neProcess, waitTime):
    try:
        referenceMatch = wait(referenceImage, waitTime)
        # print debug information
        print ("TestFunctions: 'reference.png' found")
        # return reference match
        return referenceMatch
    except:
        neProcess.kill()
        # print debug information
        sys.exit("TestFunctions: Killed netedit process. 'reference.png' not found")


"""
@brief setup and start netedit
"""


def setupAndStart(testRoot, extraParameters=[], debugInformation=True, searchReference=True, waitTime=DELAY_REFERENCE):
    setup(testRoot)
    # Open netedit
    neteditProcess = Popen(extraParameters, debugInformation)
    # atexit.register(quit, neteditProcess, False, False)
    # print debug information
    print("TestFunctions: Netedit opened sucesfully")
    # Check if reference must be searched
    if(searchReference):
        # Wait for netedit reference
        return neteditProcess, getReferenceMatch(neteditProcess, waitTime)
    else:
        # print debug information
        print("TestFunctions: 'searchReference' option disabled. Reference isn't searched")
        # Wait 1 second for netedit process
        wait(1)
        return neteditProcess

"""
@brief rebuild network
"""


def rebuildNetwork():
    typeKey(Key.F5)


"""
@brief rebuild network with volatile options
"""


def rebuildNetworkWithVolatileOptions(question=True):
    typeTwoKeys(Key.F5, Key.SHIFT)
    # confirm recompute
    if question == True:
        waitQuestion('y')
    else:
        waitQuestion('n')


"""
@brief clean junction
"""


def cleanJunction():
    typeKey(Key.F6)


"""
@brief join selected junctions
"""


def joinSelectedJunctions():
    typeKey(Key.F7)


"""
@brief select focus on upper element of current frame
"""


def focusOnFrame():
    typeKey(Key.F12)


"""
@brief undo last operation
"""


def undo(match, number):
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey("i")
    # click over match
    leftClick(match, 0, 0)
    for x in range(0, number):
        typeTwoKeys("z", Key.CTRL)


"""
@brief undo last operation
"""


def redo(match, number):
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey("i")
    # click over match
    leftClick(match, 0, 0)
    for x in range(0, number):
        typeTwoKeys("y", Key.CTRL)


"""
@brief set Zoom
"""


def setZoom(positionX, positionY, zoomLevel):
    # open edit viewport dialog
    typeKey("v")
    # Paste Zoom Z
    pasteIntoTextField(zoomLevel)
    # go to Y
    typeInvertTab()
    # Paste Position Y
    pasteIntoTextField(positionY)
    # go to X
    typeInvertTab()
    # Paste position X
    pasteIntoTextField(positionX)
    # press OK Button using shortcut
    typeTwoKeys('o', Key.ALT)


"""
@brief wait question of netedit and select a yes/no answer
"""


def waitQuestion(answer):
    # wait 0.5 second to question dialog
    wait(DELAY_QUESTION)
    # Answer can be "y" or "n"
    typeTwoKeys(answer, Key.ALT)


"""
@brief quit netedit quit
"""


def quit(neteditProcess, openNetNonSavedDialog=False, saveNet=False, 
                         openAdditionalsNonSavedDialog=False, saveAdditionals=False, 
                         openShapesNonSavedDialog=False, saveShapes=False):
    # check if netedit is already closed
    if neteditProcess.poll() is not None:
        # print debug information
        print("[log] TestFunctions: Netedit already closed")
    else:
        # quit using hotkey
        typeTwoKeys("q", Key.CTRL)

        # Check if net must be saved
        if openNetNonSavedDialog:
            # Wait some seconds
            wait(DELAY_QUESTION)
            if saveNet:
                waitQuestion("s")
            else:
                waitQuestion("q")

        # Check if additionals must be saved
        if openAdditionalsNonSavedDialog:
            # Wait some seconds
            wait(DELAY_QUESTION)
            if saveAdditionals:
                waitQuestion("s")
            else:
                waitQuestion("q")
                
        # Check if additionals must be saved
        if openShapesNonSavedDialog:
            # Wait some seconds
            wait(DELAY_QUESTION)
            if saveShapes:
                waitQuestion("s")
            else:
                waitQuestion("q")

        # wait some seconds
        wait(DELAY_QUIT)
        if neteditProcess.poll() is not None:
            # print debug information
            print("TestFunctions: Netedit closed successfully")
        else:
            neteditProcess.kill()
            # print debug information
            print("TestFunctions: Error closing Netedit")


"""
@brief save network
"""


def saveNetwork():
    # save newtork using hotkey
    typeTwoKeys("s", Key.CTRL)


"""
@brief save additionals
"""


def saveAdditionals():
    # save additionals using hotkey
    typeTwoKeys("d", Key.CTRL + Key.SHIFT)


"""
@brief save shapes
"""


def saveShapes():
    # save additionals using hotkey
    typeTwoKeys("p", Key.CTRL + Key.SHIFT)


"""
@brief open and close about dialog
"""


def openAboutDialog(waitingTime=DELAY_QUESTION):
    # type F2 to open about dialog
    typeKey(Key.F2)
    # wait before closing
    wait(waitingTime)
    # press enter to close dialog (Ok must be focused)
    typeSpace()

#################################################
# Create nodes and edges
#################################################

""" 
@brief Change to create edge mode
"""


def createEdgeMode():
    typeKey("e")


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
    for x in range(0, 3):
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
    for x in range(0, 2):
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
    typeKey("i")

"""
@brief modify attribute of type int/float/string
"""


def modifyAttribute(attributeNumber, value):
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for x in range(0, attributeNumber + 1):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type Enter to commit change
    typeEnter()


"""
@brief modify boolean attribute
"""


def modifyBoolAttribute(attributeNumber):
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for x in range(0, attributeNumber + 1):
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
    typeKey("m")


"""    
@brief move element
"""


def moveElement(match, startX, startY, endX, endY):
    # change mouse move delay
    Settings.MoveMouseDelay = 0.5
    # move element
    dragDrop(match, startX, startY, endX, endY)
    # set back mouse move delay
    Settings.MoveMouseDelay = 0.2

#################################################
# crossings
#################################################

"""
@brief Change to crossing mode
"""


def crossingMode():
    typeKey("r")


"""
@brief create crossing
"""


def createCrossing():
    # focus current frame
    focusOnFrame()
    # jump to create crossing button
    for x in range(0, 8):
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
    for x in range(0, numtabs + 1):
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
    for x in range(0, numtabs + 1):
        typeTab()
    # type space to change value
    typeSpace()


"""
@brief clear crossing
"""


def crossingClearEdges(useSelectedEdges=False):
    # focus current frame
    focusOnFrame()
    if(useSelectedEdges):
        # jump to clear button
        for x in range(0, 1):
            typeTab()
    else:
        # jump to clear button
        for x in range(0, 2):
            typeTab()
    # type space to activate button
    typeSpace()


"""
@brief invert crossing
"""


def crossingInvertEdges(useSelectedEdges=False):
    # focus current frame
    focusOnFrame()
    if(useSelectedEdges):
        # jump to clear button
        for x in range(0, 2):
            typeTab()
    else:
        # jump to clear button
        for x in range(0, 3):
            typeTab()
    # type space to activate button
    typeSpace()

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
    # go to length textfield
    for x in range(0, numTabs + 1):
        typeTab()
    # paste new lenght
    pasteIntoTextField(length)
    # type enter to save new lenght
    typeEnter()


"""
@brief modify default boolean value of an additional
"""


def modifyAdditionalDefaultBoolValue(numTabs):
    # focus current frame
    focusOnFrame()
    # place cursor in check Box position
    for x in range(numTabs + 1):
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
    for x in range(0, numTabs + 1):
        typeTab()
    # add lines using space
    for x in range(0, numLines):
        typeSpace()


"""
@brief fill lines to stopping places
"""


def fillStoppingPlaceLines(numTabs, numLines):
    # focus current frame
    focusOnFrame()
    # place cursor in the first line
    for x in range(0, numTabs + 1):
        typeTab()
    # fill lines
    for x in range(0, numLines):
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
    for x in range(0, numTabs + 1):
        typeTab()
    # select child
    if(childNumber == 0):
        typeSpace()
    else:
        for x in range(0, childNumber):
            typeKey(Key.DOWN)
    # use TAB to select additional child
    typeTab()


"""
@brief fix stoppingPlaces
"""


def fixStoppingPlace(solution):
    # select bullet depending of solution
    if (solution == "saveInvalids"):
        for x in range(0, 3):
            typeInvertTab()
        typeSpace()
        # go back and press accept
        for x in range(0, 3):
            typeTab()
        typeSpace()
    elif (solution == "fixPositions"):
        for x in range(0, 2):
            typeInvertTab()
        typeSpace()
        # go back and press accept
        for x in range(0, 2):
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
    typeKey("d")


"""
@brief Enable or disable 'automatically delete Additionals'
"""


def changeAutomaticallyDeleteAdditionals(match):
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
    wait(DELAY_QUESTION)
    # press enter to close dialog
    typeEnter()

#################################################
# select mode
#################################################

"""
@brief Change to select mode
"""


def selectMode():
    typeKey("s")


"""
@brief abort current selection
"""


def abortSelection():
    # type ESC to abort current selection
    typeEscape()


"""
@brief select elements with default frame values
"""


def selectDefault():
    for x in range(0, 19):
        typeTab()
    # type enter to select it
    typeEnter()


"""
@brief save selection
"""


def saveSelection():
    focusOnFrame()
    #jump to save
    for x in range(0, 24):
        typeTab()
    typeSpace()
    # jump to filename textfield
    typeTwoKeys("f", Key.ALT)
    filename = os.path.join(textTestSandBox, "selection.txt")
    pasteIntoTextField(filename)
    typeEnter()
    
    
"""
@brief select items
"""


def selectItems(elementClass, elementType, attribute, value):
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for x in range(0, 13):
        typeTab()
    # paste the new elementClass
    pasteIntoTextField(elementClass)
    # jump to element
    for x in range(0, 2):
        typeTab()
    # paste the new elementType
    pasteIntoTextField(elementType)
    # jump to attribute
    for x in range(0, 2):
        typeTab()
    # paste the new attribute
    pasteIntoTextField(attribute)
    # jump to value
    for x in range(0, 2):
        typeTab()
    # paste the new value
    pasteIntoTextField(value)
    # type enter to select it
    typeEnter()


"""
@brief delete selected items
"""


def deleteSelectedItems():
    typeKey(Key.DELETE)


"""
@brief set modification mode "add"
"""


def modificationModeAdd():
    # focus current frame
    focusOnFrame()
    # jump to mode "add"
    for x in range(0, 9):
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
    for x in range(0, 10):
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
    for x in range(0, 11):
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
    for x in range(0, 12):
        typeTab()
    # select it
    typeSpace()


"""
@brief select using an rectangle
"""


def selectionRectangle(match, startX, startY, endX, endY):
    # Leave Shift key pressed (Sikulix function)
    keyDown(Key.SHIFT)
    # change mouse move delay
    Settings.MoveMouseDelay = 0.5
    # move element
    dragDrop(match, startX, startY, endX, endY)
    # set back mouse move delay
    Settings.MoveMouseDelay = 0.2
    # Release Shift key (Sikulix function)
    keyUp(Key.SHIFT)


"""
@brief clear selection
"""


def selectionClear(previouslyInserted=False):
    # focus current frame
    focusOnFrame()
    for x in range(0, 22):
        typeTab()
    # type space to select clear option
    typeSpace()


"""
@brief invert selection
"""


def selectionInvert():
    # focus current frame
    focusOnFrame()
    for x in range(0, 23):
        typeTab()
    # type space to select invert opetion
    typeSpace()


"""
@brief Toogle select edges and lanes
"""


def selectionToogleEdges():
    # go to check box "select edges"
    for x in range(0, 3):
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
    typeKey("t")


"""
@brief Create TLS
"""


def createTLS():
    # focus current frame
    focusOnFrame()
    # type tab to jump to create tls button
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

def createSquaredPoly(match, positionx, positiony, size, close):
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(match, positionx, positiony)
    leftClick(match, positionx, positiony - (size/2))
    leftClick(match, positionx - (size/2), positiony - (size/2))
    leftClick(match, positionx - (size/2), positiony)
    # check if polygon has to be closed
    if(close == True):
        leftClick(match, positionx, positiony)
    # finish draw
    typeEnter()


"""
@brief Create rectangled Polygon in position with a certain size
"""

def createRectangledPoly(match, positionx, positiony, sizex, sizey, close):
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(match, positionx, positiony)
    leftClick(match, positionx, positiony - (sizey/2))
    leftClick(match, positionx - (sizex/2), positiony - (sizey/2))
    leftClick(match, positionx - (sizex/2), positiony)
    # check if polygon has to be closed
    if(close == True):
        leftClick(match, positionx, positiony)
    # finish draw
    typeEnter()


"""
@brief Create line Polygon in position with a certain size
"""

def createLinePoly(match, positionx, positiony, sizex, sizey, close):
    # focus current frame
    focusOnFrame()
    # start draw
    typeEnter()
    # create polygon
    leftClick(match, positionx, positiony)
    leftClick(match, positionx - (sizex/2), positiony - (sizey/2))
    # check if polygon has to be closed
    if(close == True):
        leftClick(match, positionx, positiony)
    # finish draw
    typeEnter()

"""
@brief modify default int/double/string value of an shape
"""


def modifyShapeDefaultValue(numTabs, value):
    # focus current frame
    focusOnFrame()
    # go to length textfield
    for x in range(0, numTabs + 1):
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
    # go to length textfield
    for x in range(0, numTabs + 1):
        typeTab()
    typeSpace()
    # go to list of colors textfield
    for x in range(2):
        typeInvertTab()
    # select color
    for x in range(1 + color):
        typeKey(Key.DOWN)
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
    for x in range(numTabs + 1):
        typeTab()
    # Change current value
    typeSpace()
