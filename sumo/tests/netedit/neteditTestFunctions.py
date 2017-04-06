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
DELAY = 0.1

Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1

neteditApp = os.environ.get("NETEDIT_BINARY", "netedit")
textTestSandBox = os.environ.get("TEXTTEST_SANDBOX", ".")
referenceImage = os.path.join("imageResources", "reference.png")

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

def Popen(newNet):
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
def setupAndStart(testRoot, newNet=False, searchReference=True, waitTime=20):
    setup(testRoot)
    # Open netedit
    neteditProcess = Popen(newNet)
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
    # wait before every operation
    wait(DELAY)
    type(Key.F5)

# select focus on upper element of current frame
def focusOnFrame():
    # wait before every operation
    wait(DELAY)
    type('f')
    
# undo last operation
def undo(match, number):
    # wait before every operation
    wait(DELAY)
    # needed to avoid errors with undo/redo (Provisionally)
    type("i")
    # wait before every operation
    wait(DELAY)
    click(match)
    for x in range(0, number):
        # wait before every operation
        wait(DELAY)
        type("z", Key.CTRL)

# undo last operation
def redo(match, number):
    # wait before every operation
    wait(DELAY)
    # needed to avoid errors with undo/redo (Provisionally)
    type("i")
    # wait before every operation
    wait(DELAY)
    click(match)
    for x in range(0, number):
        # wait before every operation
        wait(DELAY)
        type("y", Key.CTRL)


# def left click over element
def leftClick(match, positionx, positiony):
    # wait before every operation
    wait(DELAY)
    click(match.getTarget().offset(positionx, positiony))

# set Zoom
def setZoom(positionX, positionY, zoomLevel):
    # wait before every operation
    wait(DELAY)
    # open edit viewport dialog
    type("v")
    # wait before every operation
    wait(DELAY)
    # go to zoom level
    type(Key.TAB, Key.SHIFT)
    # Paste zoomLevel
    paste(zoomLevel)
    # wait before every operation
    wait(DELAY)
    # go to Y
    type(Key.TAB, Key.SHIFT)
    # Paste positionY
    paste(positionY)
    # wait before every operation
    wait(DELAY)
    # go to X
    type(Key.TAB, Key.SHIFT)
    # Paste positionX
    paste(positionX)
    # wait before every operation
    wait(DELAY)
    # Go to OK Button
    for t in xrange(3):
        type(Key.TAB)
    # wait 0.1 second before press (To avoid errors in linux)
    wait(DELAY)
    type(Key.SPACE)

# netedit wait question
def waitQuestion(answer):
    # wait 0.5 second to question dialog
    wait(0.5)
    # Answer can be "y" or "n"
    type(answer, Key.ALT)

# netedit quit
def quit(neteditProcess, mustBeSaved, save):
    if neteditProcess.poll() is not None:
        # already quit
        return

    # quit using hotkey
    type("q", Key.CTRL)

    # Check if net must be saved
    if mustBeSaved:
        if save:
            waitQuestion("y")
        else:
            waitQuestion("n")

    # wait some seconds
    for t in xrange(3):
        wait(t)
        if neteditProcess.poll() is not None:
            print("[log] netedit closed successfully")
            return
    neteditProcess.kill()
    print("error closing netedit")

# save network
def saveNetwork():
    # wait before every operation
    wait(DELAY)
    # save newtork using hotkey
    type("s", Key.CTRL)

# save additionals
def saveAdditionals():
    # wait before every operation
    wait(DELAY)
    # save additionals using hotkey
    type("d", Key.CTRL + Key.SHIFT)

#################################################
# Create edge
#################################################

# Change to delete  mode
def createEdgeMode():
    # wait before every operation
    wait(DELAY)
    type("e")

# Cancel current created edge (used in chain mode)
def cancelEdge():
    # wait before every operation
    wait(DELAY)
    type(Key.ESC)

# Change chain option
def changeChainOption():
    # wait before every operation
    wait(DELAY)
    # cancel current created edge
    cancelEdge()
    # jump to chain
    for x in range(0, 3):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)
        # wait before every operation
    wait(DELAY)
    # change chain mode
    type(Key.SPACE)

# Change two-way mode
def changeTwoWayOption():
    # wait before every operation
    wait(DELAY)
    # cancel current created edge
    cancelEdge()
    # jump to two way
    for x in range(0, 2):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)
    # wait before every operation
    wait(DELAY)
    # change two way mode
    type(Key.SPACE)
    
#################################################
# Inspect mode
#################################################

# go to inspect mode
def inspectMode():
    # wait before every operation
    wait(DELAY)
    type("i")

# netedit modify int/float/string
def modifyAttribute(attributeNumber, value):
    # wait before every operation
    wait(DELAY)
    type("i")
    # jump to attribute
    for x in range(0, attributeNumber + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select all values
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new value
    paste(value)
    # wait before every operation
    wait(DELAY)
    # type ESC to save changea and avoid edit accidentally
    type(Key.ESC)

# netedit modify bool attribute
def modifyBoolAttribute(attributeNumber):
    # wait before every operation
    wait(DELAY)
    type("i")
    # jump to attribute
    for x in range(0, attributeNumber + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type SPACE to change value
    type(Key.SPACE)
    
#################################################
# Move mode
#################################################

# set move mode
def moveMode():
    # wait before every operation
    wait(DELAY)
    type("m")

# move element
def moveElement(match, startX, startY, endX, endY):
    # change mouse move delay
    Settings.MoveMouseDelay = 0.5
    # move element
    dragDrop(match.getTarget().offset(startX, startY),
             match.getTarget().offset(endX, endY))
    # set back mouse move delay
    Settings.MoveMouseDelay = 0.1

#################################################
# crossings
#################################################

# Change to crossing mode
def crossingMode():
    # wait before every operation
    wait(DELAY)
    type("r")

# create crossing
def createCrossing():
    # focus current frame
    focusOnFrame()
    # jump to create crossing button
    for x in range(0, 8):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type space to create crossing
    type(Key.SPACE)
    # go back to avoid errors
    for x in range(0, 8):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)

# change default int/real/string crossing default value
def modifyCrossingDefaultValue(numtabs, value):
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, numtabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # select all value
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new value
    paste(value)
    # wait before every operation
    wait(DELAY)
    # type enter to save change
    type(Key.ENTER)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid accidentally changes
    type(Key.ESC)

# change default boolean crossing default value
def modifyCrossingDefaultBoolValue(numtabs):
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, numtabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type space to change value
    type(Key.SPACE)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid accidentally changes
    type(Key.ESC)
    
# clear crossing
def crossingClearEdges(useSelectedEdges = False):
    # focus current frame
    focusOnFrame()
    if(useSelectedEdges):
        # jump to clear button
        for x in range(0, 1):
            # wait before every operation
            wait(DELAY)
            type(Key.TAB)
    else:
        # jump to clear button
        for x in range(0, 2):
            # wait before every operation
            wait(DELAY)
            type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type space to activate button
    type(Key.SPACE)

#invert crossing
def crossingInvertEdges(useSelectedEdges = False):
    # focus current frame
    focusOnFrame()
    if(useSelectedEdges):
        # jump to clear button
        for x in range(0, 2):
            # wait before every operation
            wait(DELAY)
            type(Key.TAB)
    else:
        # jump to clear button
        for x in range(0, 3):
            # wait before every operation
            wait(DELAY)
            type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type space to activate button
    type(Key.SPACE)

#################################################
# additionals
#################################################

# change to additional mode
def additionalMode():
    # wait before every operation
    wait(DELAY)
    type('a')

# change additional
def changeAdditional(additional):
    # focus current frame
    focusOnFrame()
    # wait before every operation
    wait(DELAY)
    # go to first editable element of frame
    type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select current value
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new value
    paste(additional)
    # wait before every operation
    wait(DELAY)
    # type enter to save change
    type(Key.ENTER)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid edit combobox accidentally
    type(Key.ESC)

# modify default int/double/string value of an additional
def modifyAdditionalDefaultValue(numTabs, length):
    # focus current frame
    focusOnFrame()
    # go to length textfield
    for x in range(0, numTabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select current value
    type("a", Key.CTRL)
        # wait before every operation
    wait(DELAY)
    # paste new lenght
    paste(length)
    # wait before every operation
    wait(DELAY)
    # type enter to save new lenght
    type(Key.ENTER)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid edit combobox accidentally
    type(Key.ESC)
    
# modify default boolean value of an additional
def modifyAdditionalDefaultBoolValue(numTabs):
    # focus current frame
    focusOnFrame()
    # place cursor in force position checkbox
    for x in range(numTabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # Change current value
    type(Key.SPACE)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid edit combobox accidentally
    type(Key.ESC)

# modify number of stopping place lines
def modifyStoppingPlaceLines(numTabs, numLines):
    # focus current frame
    focusOnFrame()
    # go to add line
    for x in range(0, numTabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # add lines
    for x in range(0, numLines):
        # wait before every operation
        wait(DELAY)
        type(Key.SPACE)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid edit lines accidentally
    type(Key.ESC)

# fill lines to stopping places
def fillStoppingPlaceLines(numTabs, numLines):
    # focus current frame
    focusOnFrame()
    # place cursor in the first line
    for x in range(0, numTabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # fill lines
    for x in range(0, numLines):
        # wait before every operation
        wait(DELAY)
        paste("Line" + str(x))
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type ESC to avoid edit combobox accidentally
    type(Key.ESC)

# select child of additional
def selectAdditionalChild(numTabs, childNumber):
    # focus current frame
    focusOnFrame()
    # place cursor in the list of childs
    for x in range(0, numTabs + 1):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # select child
    if(childNumber == 0):
        # wait before every operation
        wait(DELAY)
        type(Key.SPACE)
    else:
        for x in range(0, childNumber):
            # wait before every operation
            wait(DELAY)
            type(Key.DOWN)
    # wait before every operation
    wait(DELAY)
    # use TAB to select additional child
    type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type ESC and key to avoid change additional child accidentally
    type(Key.ESC)


#################################################
# delete
#################################################

# Change to delete mode
def deleteMode():
    # wait before every operation
    wait(DELAY)
    type("d")

# Enable or disable 'automatically delete Additionals'
def changeAutomaticallyDeleteAdditionals(match):
    click(match.getTarget().offset(-120, 100))

# close warning about automatically delete additionals
def waitAutomaticallyDeleteAdditionalsWarning():
    # wait 0.5 second to question dialog
    wait(0.5)
    # press enter to close dialog
    type(Key.ENTER)

#################################################
# select mode
#################################################

# Change to select mode
def selectMode():
    # wait before every operation
    wait(DELAY)
    type("s")    

# abort current selection
def abortSelection():
    # wait before every operation
    wait(DELAY)
    type(Key.ESC)

# select elements with default frame values
def selectDefault():
    for x in range(0, 11):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    type(Key.ENTER)
    
# select items
def selectItems(elementClass, elementType, attribute, value):
    # focus current frame
    focusOnFrame()
    #jump to elementClass
    for x in range(0, 5):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new elementClass
    paste(elementClass)
    # jump to element
    for x in range(0, 2):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select all
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new elementType
    paste(elementType)
    # jump to attribute
    for x in range(0, 2):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select all
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new attribute
    paste(attribute)
    # jump to value
    for x in range(0, 2):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select all
    type("a", Key.CTRL)
    # wait before every operation
    wait(DELAY)
    # paste the new value
    paste(value)
    # wait before every operation
    wait(DELAY)
    # type enter to select it
    type(Key.ENTER)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 10):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)

# delete selected items
def deleteSelectedItems():
    # wait before every operation
    wait(DELAY)
    type(Key.DELETE)
    
# set modification mode "add"
def modificationModeAdd():
    # focus current frame
    focusOnFrame()
    # wait before every operation
    wait(DELAY)
    # go to first editable element of frame
    type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select it
    type(Key.SPACE)
    # wait before every operation
    wait(DELAY)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    type(Key.TAB, Key.SHIFT)
    
# set modification mode "remove"
def modificationModeRemove():
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, 2):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select it
    type(Key.SPACE)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 2):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)
    
# set modification mode "keep"
def modificationModeKeep():
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, 3):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select it
    type(Key.SPACE)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 3):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)
    
# set modification mode "replace"
def modificationModeReplace():
    # focus current frame
    focusOnFrame()
    # jump to value
    for x in range(0, 4):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # select it
    type(Key.SPACE)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 4):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)

# select using an rectangle
def selectionRectangle(match, startX, startY, endX, endY):
    keyDown(Key.SHIFT)
    # change mouse move delay
    Settings.MoveMouseDelay = 0.5
    # move element
    dragDrop(match.getTarget().offset(startX, startY), match.getTarget().offset(endX, endY))
    # set back mouse move delay
    Settings.MoveMouseDelay = 0.1
    keyUp(Key.SHIFT)

# clear selection
def selectionClear(previouslyInserted = False):
    # focus current frame
    focusOnFrame()
    for x in range(0, 14):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type space to select clear option
    type(Key.SPACE)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 11):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)

# invert selection
def selectionInvert():
    # focus current frame
    focusOnFrame()
    for x in range(0, 15):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB)
    # wait before every operation
    wait(DELAY)
    # type space to select invert opetion
    type(Key.SPACE)
    # go back to avoid focus errors (ESC cannot be used because unselect selected items)
    for x in range(0, 12):
        # wait before every operation
        wait(DELAY)
        type(Key.TAB, Key.SHIFT)

#################################################
# traffic light
#################################################

# Change to traffic light mode
def selectTLS():
    # wait before every operation
    wait(DELAY)
    type("t")    