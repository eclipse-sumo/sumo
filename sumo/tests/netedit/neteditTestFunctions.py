#!/usr/bin/env python
"""
@file    neteditTestFunctions.py
@author  Pablo Alvarez Lopez
@date    2016-11-25
@version $Id$

Simplify writing of sikulix test scripts for netedit

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR/TS, Germany

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
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

Settings.MoveMouseDelay = 0.2
Settings.DelayBeforeDrop = 0.2
Settings.DelayAfterDrag = 0.2

neteditApp = os.environ.get("NETEDIT_BINARY", "netedit")
textTestSandBox = os.environ.get("TEXTTEST_SANDBOX", ".")
referenceImage = os.path.join("imageResources", "reference.png")

#################################################
# interaction functions
#################################################

# type escape key


def typeEscape():
    # wait before every operation
    wait(DELAY)
    type(Key.ESC)

# type enter  key


def typeEnter():
    # wait before every operation
    wait(DELAY)
    type(Key.ENTER)

# type space key


def typeSpace():
    wait(DELAY)
    type(Key.SPACE)

# type tab key


def typeTab():
    # wait before every operation
    wait(DELAY)
    type(Key.TAB)

# type Shift + Tab keys


def typeInvertTab():
    # wait before every operation
    wait(DELAY)
    type(Key.TAB, Key.SHIFT)

# type single key


def typeKey(key):
    # wait before every operation
    wait(DELAY)
    type(key)

# type two keys at the same time


def typeTwoKeys(key1, key2):
    # wait before every operation
    wait(DELAY)
    type(key1, key2)

# paste value into current text field


def pasteIntoTextField(value):
    # wait before every operation
    wait(DELAY)
    paste(value)

# do left click over a position relative to match (pink square)


def leftClick(match, positionx, positiony):
    # wait before every operation
    wait(DELAY)
    # click respect to offset
    click(match.getTarget().offset(positionx, positiony))

#################################################
# basic functions
#################################################


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


def Popen(newNet, extraParameters):
    # set the default parameters of netedit
    neteditCall = [neteditApp, '--gui-testing', '--window-pos', '50,50',
                   '--window-size', '700,500', '--no-warnings',
                   '--error-log', os.path.join(textTestSandBox, 'log.txt')]

    # check if a new net must be created
    if newNet:
        neteditCall += ['--new']

    # check if an existent net must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_net.net.xml")):
        neteditCall += ['--sumo-net-file',
                        os.path.join(textTestSandBox, "input_net.net.xml")]

    # set output for net
    neteditCall += ['--output-file',
                    os.path.join(textTestSandBox, 'net.net.xml')]

    # Check if additionals must be loaded (additionals output will be
    # automatically set)
    if os.path.exists(os.path.join(textTestSandBox, "input_additionals.add.xml")):
        neteditCall += ['--sumo-additionals-file',
                        os.path.join(textTestSandBox, "input_additionals.add.xml")]

    # set output for additionals
    neteditCall += ['--additionals-output',
                    os.path.join(textTestSandBox, "additionals.xml")]

    # check if a gui settings file has to be load
    if os.path.exists(os.path.join(textTestSandBox, "gui-settings.xml")):
        neteditCall += ['--gui-settings-file',
                        os.path.join(textTestSandBox, "gui-settings.xml")]

    # check if extra parameters has to be added
    neteditCall += extraParameters
        
    # return a subprocess with netedit
    return subprocess.Popen(neteditCall, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# obtain match


def getReferenceMatch(neProcess, waitTime):
    try:
        return wait(referenceImage, waitTime)
    except:
        neProcess.kill()
        sys.exit("Killed netedit process. 'reference.png' not found")

# setup and start netedit


def setupAndStart(testRoot, newNet=False, searchReference=True, extraParameters = [], waitTime=DELAY_REFERENCE):
    setup(testRoot)
    # Open netedit
    neteditProcess = Popen(newNet, extraParameters)
    atexit.register(quit, neteditProcess, False, False)
    # Wait for netedit reference
    if(searchReference):
        # Wait for netedit reference
        return neteditProcess, getReferenceMatch(neteditProcess, waitTime)
    else:
        # Wait 1 second for netedit
        wait(1)
        return neteditProcess

# rebuild network


def rebuildNetwork():
    typeKey(Key.F5)

    
# rebuild network with volatile options

def rebuildNetworkWithVolatileOptions(question = True):
    typeTwoKeys(Key.F5, Key.SHIFT)
    #confirm recompute
    if question == True:
        waitQuestion('y')
    else:
        waitQuestion('n')
    
    
# clean junction

def cleanJunction():
    typeKey(Key.F6)

# join selected junctions

    
def joinSelectedJunctions():
    typeKey(Key.F7)
    
# select focus on upper element of current frame


def focusOnFrame():
    typeKey('f')

# undo last operation


def undo(match, number):
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey("i")
    # click over match
    leftClick(match, 0, 0)
    for x in range(0, number):
        typeTwoKeys("z", Key.CTRL)

# undo last operation


def redo(match, number):
    # needed to avoid errors with undo/redo (Provisionally)
    typeKey("i")
    # click over match
    leftClick(match, 0, 0)
    for x in range(0, number):
        typeTwoKeys("y", Key.CTRL)

# set Zoom


def setZoom(positionX, positionY, zoomLevel):
    # open edit viewport dialog
    typeKey("v")
    # go to zoom level
    typeInvertTab()
    # Paste zoomLevel
    pasteIntoTextField(zoomLevel)
    # go to Y
    typeInvertTab()
    # Paste positionY
    pasteIntoTextField(positionY)
    # go to X
    typeInvertTab()
    # Paste positionX
    pasteIntoTextField(positionX)
    # Go to OK Button
    for t in xrange(3):
        typeTab()
    # press OK Button
    typeSpace()

# netedit wait question


def waitQuestion(answer):
    # wait 0.5 second to question dialog
    wait(DELAY_QUESTION)
    # Answer can be "y" or "n"
    typeTwoKeys(answer, Key.ALT)

# netedit quit


def quit(neteditProcess, netNonSavedDialog=False, saveNet=False, additionalsNonSavedDialog=False, saveAdditionals=False):
    if neteditProcess.poll() is not None:
        # already quit
        return

    # quit using hotkey
    typeTwoKeys("q", Key.CTRL)

    # Check if net must be saved
    if netNonSavedDialog:
        if saveNet:
            waitQuestion("s")
        else:
            waitQuestion("q")

    # Check if additionals must be saved
    if additionalsNonSavedDialog:
        if saveAdditionals:
            waitQuestion("s")
        else:
            waitQuestion("q")

    # wait some seconds
    for t in xrange(3):
        waitQuestion("q")
        waitQuestion("q")
        wait(t)
        if neteditProcess.poll() is not None:
            print("[log] netedit closed successfully")
            return
    neteditProcess.kill()
    print("error closing netedit")

# save network


def saveNetwork():
    # save newtork using hotkey
    typeTwoKeys("s", Key.CTRL)

# save additionals


def saveAdditionals():
    # save additionals using hotkey
    typeTwoKeys("d", Key.CTRL + Key.SHIFT)


# open and close about dialog
def openAboutDialog(waitingTime = DELAY_QUESTION):
    # type F2 to open about dialog
    typeKey(Key.F2)
    # wait before closing
    wait(waitingTime)
    # press enter to close dialog (Ok must be focused)
    typeSpace()

    
#################################################
# Create edge
#################################################

# Change to create edge mode


def createEdgeMode():
    typeKey("e")

# Cancel current created edge (used in chain mode)


def cancelEdge():
    # type ESC to cancel current edge
    typeEscape()

# Change chain option


def changeChainOption():
    # cancel current created edge
    cancelEdge()
    # jump to chain
    for x in range(0, 3):
        typeInvertTab()
    # change chain mode
    typeSpace()

# Change two-way mode


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

# go to inspect mode


def inspectMode():
    typeKey("i")

# netedit modify int/float/string


def modifyAttribute(attributeNumber, value):
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for x in range(0, attributeNumber + 1):
        typeTab()
    # select all values
    typeTwoKeys("a", Key.CTRL)
    # paste the new value
    pasteIntoTextField(value)
    # type ESC to commit change and avoid edit accidentally
    typeEscape()

# netedit modify bool attribute


def modifyBoolAttribute(attributeNumber):
    # focus current frame
    focusOnFrame()
    # jump to attribute
    for x in range(0, attributeNumber + 1):
        typeTab()
    # type SPACE to change value
    typeSpace()
    # type ESC to quit focus of checkBox
    typeEscape()

#################################################
# Move mode
#################################################

# set move mode


def moveMode():
    typeKey("m")

# move element


def moveElement(match, startX, startY, endX, endY):
    # change mouse move delay
    Settings.MoveMouseDelay = 0.5
    # move element
    dragDrop(match.getTarget().offset(startX, startY),
             match.getTarget().offset(endX, endY))
    # set back mouse move delay
    Settings.MoveMouseDelay = 0.2

#################################################
# crossings
#################################################

# Change to crossing mode


def crossingMode():
    typeKey("r")

# create crossing


def createCrossing():
    # focus current frame
    focusOnFrame()
    # jump to create crossing button
    for x in range(0, 8):
        typeTab()
    # type space to create crossing
    typeSpace()
    # go back to avoid errors
    for x in range(0, 8):
        typeInvertTab()

# change default int/real/string crossing default value


def modifyCrossingDefaultValue(numtabs, value):
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, numtabs + 1):
        typeTab()
    # select all value
    typeTwoKeys("a", Key.CTRL)
    # paste the new value
    pasteIntoTextField(value)
    # type enter to save change
    typeEnter()
    # type ESC to avoid accidentally changes
    typeEscape()

# change default boolean crossing default value


def modifyCrossingDefaultBoolValue(numtabs):
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, numtabs + 1):
        typeTab()
    # type space to change value
    typeSpace()
    # type ESC to avoid accidentally changes
    typeEscape()

# clear crossing


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

# invert crossing


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

# change to additional mode


def additionalMode():
    typeKey('a')

# change additional


def changeAdditional(additional):
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    typeTab()
    # select current value
    typeTwoKeys("a", Key.CTRL)
    # paste the new value
    pasteIntoTextField(additional)
    # type enter to save change
    typeEnter()
    # type ESC to avoid edit combobox accidentally
    typeEscape()

# modify default int/double/string value of an additional


def modifyAdditionalDefaultValue(numTabs, length):
    # focus current frame
    focusOnFrame()
    # go to length textfield
    for x in range(0, numTabs + 1):
        typeTab()
    # select current value
    typeTwoKeys("a", Key.CTRL)
    # paste new lenght
    pasteIntoTextField(length)
    # type enter to save new lenght
    typeEnter()
    # type ESC to avoid edit combobox accidentally
    typeEscape()

# modify default boolean value of an additional


def modifyAdditionalDefaultBoolValue(numTabs):
    # focus current frame
    focusOnFrame()
    # place cursor in force position checkbox
    for x in range(numTabs + 1):
        typeTab()
    # Change current value
    typeSpace()
    # type ESC to avoid edit combobox accidentally
    typeEscape()

# modify number of stopping place lines


def modifyStoppingPlaceLines(numTabs, numLines):
    # focus current frame
    focusOnFrame()
    # go to add line
    for x in range(0, numTabs + 1):
        typeTab()
    # add lines using space
    for x in range(0, numLines):
        typeSpace()
    # type ESC to avoid edit lines accidentally
    typeEscape()

# fill lines to stopping places


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
    # type ESC to avoid edit combobox accidentally
    typeEscape()

# select child of additional


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
    # type ESC and key to avoid change additional child accidentally
    typeEscape()


#################################################
# delete
#################################################

# Change to delete mode
def deleteMode():
    typeKey("d")

# Enable or disable 'automatically delete Additionals'


def changeAutomaticallyDeleteAdditionals(match):
    # select delete mode again to set mode
    deleteMode()
    # use TAB to go to check box
    typeTab()
    # type SPACE to change value
    typeSpace()

# close warning about automatically delete additionals


def waitAutomaticallyDeleteAdditionalsWarning():
    # wait 0.5 second to question dialog
    wait(DELAY_QUESTION)
    # press enter to close dialog
    typeEnter()

#################################################
# select mode
#################################################

# Change to select mode


def selectMode():
    typeKey("s")

# abort current selection


def abortSelection():
    # type ESC to abort current selection
    typeEscape()

# select elements with default frame values


def selectDefault():
    for x in range(0, 11):
        typeTab()
    # type enter to select it
    typeEnter()

# select items


def selectItems(elementClass, elementType, attribute, value):
    # focus current frame
    focusOnFrame()
    # jump to elementClass
    for x in range(0, 5):
        typeTab()
    # select all
    typeTwoKeys("a", Key.CTRL)
    # paste the new elementClass
    pasteIntoTextField(elementClass)
    # jump to element
    for x in range(0, 2):
        typeTab()
    # select all
    typeTwoKeys("a", Key.CTRL)
    # paste the new elementType
    pasteIntoTextField(elementType)
    # jump to attribute
    for x in range(0, 2):
        typeTab()
    # select all
    typeTwoKeys("a", Key.CTRL)
    # paste the new attribute
    pasteIntoTextField(attribute)
    # jump to value
    for x in range(0, 2):
        typeTab()
    # select all
    typeTwoKeys("a", Key.CTRL)
    # paste the new value
    pasteIntoTextField(value)
    # type enter to select it
    typeEnter()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 10):
        typeInvertTab()

# delete selected items


def deleteSelectedItems():
    typeKey(Key.DELETE)

# set modification mode "add"


def modificationModeAdd():
    # focus current frame
    focusOnFrame()
    # go to first editable element of frame
    typeTab()
    # select it
    typeSpace()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    typeInvertTab()

# set modification mode "remove"


def modificationModeRemove():
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, 2):
        typeTab()
    # select it
    typeSpace()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 2):
        typeInvertTab()

# set modification mode "keep"


def modificationModeKeep():
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, 3):
        typeTab()
    # select it
    typeSpace()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 3):
        typeInvertTab()

# set modification mode "replace"


def modificationModeReplace():
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, 4):
        typeTab()
    # select it
    typeSpace()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 4):
        typeInvertTab()

# select using an rectangle


def selectionRectangle(match, startX, startY, endX, endY):
    keyDown(Key.SHIFT)
    # change mouse move delay
    Settings.MoveMouseDelay = 0.5
    # move element
    dragDrop(match.getTarget().offset(startX, startY), match.getTarget().offset(endX, endY))
    # set back mouse move delay
    Settings.MoveMouseDelay = 0.2
    keyUp(Key.SHIFT)

# clear selection


def selectionClear(previouslyInserted=False):
    # focus current frame
    focusOnFrame()
    for x in range(0, 14):
        typeTab()
    # type space to select clear option
    typeSpace()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 11):
        typeInvertTab()

# invert selection


def selectionInvert():
    # focus current frame
    focusOnFrame()
    for x in range(0, 15):
        typeTab()
    # type space to select invert opetion
    typeSpace()
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 12):
        typeInvertTab()

#################################################
# traffic light
#################################################

# Change to traffic light mode


def selectTLS():
    typeKey("t")
