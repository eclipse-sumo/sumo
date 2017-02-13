# Import libraries
import os
import sys
import subprocess
import platform
import atexit
from sikuli import *

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
    neteditCall = [neteditApp, '--gui-testing', '--window-size', '700,500',
                   '--no-warnings', '--error-log', os.path.join(textTestSandBox, 'log.txt')]

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
    type(Key.F5)


# netedit undo
def undo(match, number):
    # needed to avoid errors with undo/redo
    inspectMode()
    click(match)
    for x in range(0, number):
        type("z", Key.CTRL)


# netedit redo
def redo(match, number):
    # needed to avoid errors with undo/redo
    inspectMode()
    click(match)
    for x in range(0, number):
        type("y", Key.CTRL)


# def left click over element
def leftClick(match, positionx, positiony):
    click(match.getTarget().offset(positionx, positiony))


# zoom in
def zoomIn(position, level):
    # set mouse over position
    hover(position)
    # apply zoom it using key +
    for x in range(level):
        type(Key.ADD)


# zoom out
def zoomOut(position, level):
    # set mouse over position
    hover(position)
    # apply zoom it using key -
    for x in range(level):
        type(Key.MINUS)


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
    # save newtork using hotkey
    type("s", Key.CTRL)


# save additionals
def saveAdditionals():
    # save additionals using hotkey
    type("d", Key.CTRL + Key.SHIFT)

#################################################
# Inspect mode
#################################################

# set inspect mode


def inspectMode():
    type("i")

# netedit parameters reference


def getParametersReference(match):
    return match.getTarget().offset(-75, 50)

# netedit modify int/float/string


def modifyAttribute(parametersReference, attributeNumber, value):
    click(parametersReference)
    # jump to attribute
    for x in range(0, attributeNumber):
        type(Key.TAB)
    # select all values
    type("a", Key.CTRL)
    # paste the new value
    paste(value)
    # type enter to save change
    type(Key.ENTER)

# netedit modify bool


def modifyBoolAttribute(parametersReference, attributeNumber):
    click(parametersReference)
    # jump to bool attribute
    for x in range(0, attributeNumber):
        type(Key.TAB)
    # change value
    type(Key.SPACE)

#################################################
# Move mode
#################################################

# set move mode


def moveMode():
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
    type("r")

# create crossing


def createCrossing(match):
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # jump to create edge
    for x in range(0, 4):
        type(Key.TAB)
    # type enter to create crossing
    type(Key.SPACE)


def modifyCrossingEdges(match, value):
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # select all values
    type("a", Key.CTRL)
    # paste the new value
    paste(value)
    # type enter to save change
    type(Key.ENTER)


def modifyCrossingPriority(match):
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # jump to priority
    type(Key.TAB)
    # type enter to save change
    type(Key.SPACE)


def modifyCrossingWidth(match, value):
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # jump to create edge
    for x in range(0, 2):
        type(Key.TAB)
    # select all values
    type("a", Key.CTRL)
    # paste the new value
    paste(value)
    # type enter to save change
    type(Key.ENTER)


def clearCrossings(match):
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # jump to clear button
    for x in range(0, 3):
        type(Key.TAB, Key.SHIFT)
    # type space to activate button
    type(Key.SPACE)


def invertCrossings(match):
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # jump to invert button
    for x in range(0, 2):
        type(Key.TAB, Key.SHIFT)
    # type space to activate button
    type(Key.SPACE)

#################################################
# additionals
#################################################

# Change to create additional mode


def additionalMode():
    type("a")


def getComboBoxAdditional(match):
    return match.getTarget().offset(-75, 50)


def changeAdditional(comboboxAdditional, number):
    click(comboboxAdditional)
    # select type of additionals depending of number
    if(number >= 0):
        for x in range(number):
            type(Key.DOWN)
    else:
        for x in range(-1 * number):
            type(Key.UP)


# block additional
def changeBlockAdditional(comboboxAdditional, numTabs):
    click(comboboxAdditional)
    # place cursor in block movement checkbox
    for x in range(numTabs):
        type(Key.TAB)
    # Change current value
    type(Key.SPACE)


def modifyStoppingPlaceLength(comboboxAdditional, numTabs, length):
    click(comboboxAdditional)
    # go to length textfield
    for x in range(0, numTabs):
        type(Key.TAB)
    # paste new lenght
    paste(length)


# force position
def modifyStoppingPlaceForcePosition(comboboxAdditional, numTabs):
    click(comboboxAdditional)
    # place cursor in force position checkbox
    for x in range(numTabs):
        type(Key.TAB)
    # Change current value
    type(Key.SPACE)


# modify stopping place reference
def modifyStoppingPlaceReference(comboboxAdditional, numTabs, numDowns):
    click(comboboxAdditional)
    # place cursor in comboBox Reference
    for x in range(numTabs):
        type(Key.TAB)
    # Set comboBox in the first element
    for x in range(3):
        type(Key.UP)
    # select new reference
    for x in range(numDowns):
        type(Key.DOWN)


# modify stopping place lines
def modifyNumberOfStoppingPlaceLines(comboboxAdditional, numTabs, numLines):
    click(comboboxAdditional)
    # go to add line
    for x in range(0, numTabs):
        type(Key.TAB)
    # add lines
    for x in range(0, numLines):
        type(Key.SPACE)


def addStoppingPlaceLines(comboboxAdditional, numTabs, numLines):
    click(comboboxAdditional)
    # place cursor in the first line
    for x in range(0, 2):
        type(Key.TAB)
    # fill lines
    for x in range(0, numLines):
        paste("Line" + str(x))
        type(Key.TAB)


def selectChild(comboboxAdditional, numTabs, childNumber):
    # first is needed to select childs with space
    click(comboboxAdditional)
    # place cursor in the list of childs
    for x in range(0, numTabs):
        type(Key.TAB)
    type(Key.SPACE)
    # now child can be selected
    click(comboboxAdditional)
    # place cursor in the list of childs
    for x in range(0, numTabs):
        type(Key.TAB)
    # select child
    for x in range(0, childNumber):
        type(Key.DOWN)


#################################################
# delete
#################################################

# Change to delete  mode
def deleteMode():
    type("d")

# Enable or disable 'automatically delete Additionals'


def changeAutomaticallyDeleteAdditionals(match):
    click(match.getTarget().offset(-120, 100))


def waitAutomaticallyDeleteAdditionalsWarning():
    # wait 0.5 second to question dialog
    wait(0.5)
    # press enter to close dialog
    type(Key.ENTER)
