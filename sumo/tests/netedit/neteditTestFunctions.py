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
    # Open current environment file to obtain path to the netedit app, textTestSandBox
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
    referenceImage = os.path.join(neteditTests, "imageResources", "reference.png")


def Popen(newNet):
    # set the default parameters of netedit
    neteditCall = [neteditApp, '--gui-testing', '--window-size', '700,500',
                   '--no-warnings', '--error-log', os.path.join(textTestSandBox, 'log.txt')]
    
    # check if a new net must be created, or a existent net must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_net.net.xml")):
        neteditCall += ['--sumo-net-file', os.path.join(textTestSandBox, "input_net.net.xml")]
    elif newNet:
        neteditCall += ['--new', '--output-file', os.path.join(textTestSandBox, 'net.net.xml')]
    
    # Check if additionals must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_additionals.add.xml")):
        neteditCall += ['--sumo-additionals-file', os.path.join(textTestSandBox, "input_additionals.add.xml")]
    else:
        neteditCall += ['--additionals-output', os.path.join(textTestSandBox, "additionals.xml")]
    
    return subprocess.Popen(neteditCall, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)


# obtain match 
def getReferenceMatch(neProcess):
    try:
        return wait(referenceImage, 20)
    except:
        neProcess.kill()
        sys.exit("Killed netedit process. 'reference.png' not found")


def setupAndStart(testRoot, newNet):
    setup(testRoot)
    # Open netedit
    neteditProcess = Popen(newNet)
    atexit.register(quit, neteditProcess, False, False)
    # Wait for netedit reference
    return neteditProcess, getReferenceMatch(neteditProcess)


def rebuildNetwork() :
	type(Key.F5)

	
# netedit undo
def undo(match, number) :
    click(match)
    for x in range(0, number) :
        type("z", Key.CTRL)


# netedit redo
def redo(match, number) :
    click(match)
    for x in range(0, number) :
        type("y", Key.CTRL)

        
# create crossing
def createCrossing(match) :
    # select edges attribute
    click(match.getTarget().offset(-100, 250))
    # jump to create edge
    for x in range(0, 4) :    
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
    for x in range(0, 2) :
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
	for x in range(0, 3) :
		type(Key.TAB, Key.SHIFT)
	# type space to activate button
	type(Key.SPACE)

def invertCrossings(match):
	# select edges attribute
	click(match.getTarget().offset(-100, 250))
	# jump to invert button
	for x in range(0, 2) :
		type(Key.TAB, Key.SHIFT)
	# type space to activate button
	type(Key.SPACE)

	
# netedit modify attribute
def modifyAttribute(parametersReference, attributeNumber, value):
    click(parametersReference)
    # jump to attribute
    for x in range(0, attributeNumber) :
        type(Key.TAB)
    # select all values
    type("a", Key.CTRL)
    # paste the new value
    paste(value)
    # type enter to save change
    type(Key.ENTER)
    

# netedit modify attribute
def modifyStoppingPlaceReference(parametersReference, numTabs, numDowns):
    click(parametersReference)
    # place cursor in comboBox Reference
    for x in range(numTabs):
        type(Key.TAB)
    # Set comboBox in the first element
    for x in range(3):
        type(Key.UP)
    # select new reference
    for x in range(numDowns):
        type(Key.DOWN)


# block additional
def changeBlockAdditional(parametersReference, numTabs):
    click(parametersReference)
    # place cursor in block movement checkbox
    for x in range(numTabs):
        type(Key.TAB)
    # Change current value
    type(Key.SPACE)
    

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
    #Answer can be "y" or "n"
    type(answer, Key.ALT)


# netedit quit
def quit(neteditProcess, mustBeSaved, save) :
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
def saveNetwork() :
    # save newtork using hotkey
    type("s", Key.CTRL)
	
# save additionals
def saveAdditionals(match) :
    # save additionals using hotkey
    type("d", Key.CTRL + Key.SHIFT)
	
