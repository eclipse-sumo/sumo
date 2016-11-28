# Import libraries
import sys
import subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1

# Get SUMO Folder
SUMOFolder = os.environ.get('SUMO_HOME', '.')
# Open current environment file to obtain path to the netedit app, textTestSandBox
currentEnvironmentFile = open(SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
# Get path to netEdit app
neteditApp = currentEnvironmentFile.readline().replace("\n", "")
# Get SandBox folder
textTestSandBox = currentEnvironmentFile.readline().replace("\n", "")
# get Current OS
currentOS = currentEnvironmentFile.readline().replace("\n", "")
# Close current environment file
currentEnvironmentFile.close()

# get reference for match
neteditReference = SUMOFolder + "/tests/netedit/imageResources/reference.png"
# Get resources depending of the current Operating system
neteditResourceEditUndo = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/edit-undo.png"
neteditResourceEditRedo = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/edit-redo.png"
neteditResourceQuestion = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/question.png"

def openNetedit(newNet) :
    # set the default parameters of netedit
    neteditParameters = [neteditApp, '--gui-testing', '--window-size', '700,500']
    
    # check if a new net must be created, or a existent net must be loaded
    if (os.path.exists(textTestSandBox + "/input_net.net.xml") == True) :
        neteditParameters.append('--sumo-net-file')
        neteditParameters.append(textTestSandBox + "/input_net.net.xml")
    elif(newNet == True) :
        neteditParameters.append('--new')
    
    # Check if additionals must be loaded
    if(os.path.exists(textTestSandBox + "/input_additionals.add.xml") == True) :
        neteditParameters.append('--sumo-additionals-file')
        neteditParameters.append(textTestSandBox + "/input_additionals.add.xml")
    else :
        neteditParameters.append('--additionals-output')
        neteditParameters.append(textTestSandBox + "/additionals.xml")
    
    return subprocess.Popen(neteditParameters, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)


# obtain match 
def getNeteditMatch(NEProcess) :
    try:
        return wait(neteditReference, 20)
    except:
        NEProcess.kill()
        sys.exit("Killed netedit process. 'reference.png' not found")

# netedit undo
def neteditUndo(NEProcess, match, number) :
    for x in range(0, number) :
        type("z", Key.CTRL)
    
# netedit redo
def neteditRedo(NEProcess, match, number) :
    for x in range(0, number) :
        type("y", Key.CTRL)

        
# netedit modify attribute
def modifyAttribute(parametersReference, attributeNumber, value) :
    click(parametersReference)
    for x in range(0, attributeNumber) :
        type(Key.TAB)
    # select all values
    type("a", Key.CTRL)
    # paste the new value
    paste(value)
    # type enter to save change
    type(Key.ENTER)
    
# netedit modify attribute
def modifyStoppingPlaceReference(parametersReference, numTabs, numDowns) :
    click(parametersReference)
    # place cursor in comboBox Reference
    for x in range(0, numTabs) :
        type(Key.TAB)
    # Set comboBox in the first element
    for x in range(0, 3) :
        type(Key.UP)
    # select new reference
    for x in range(0, numDowns) :
        type(Key.DOWN)
    
# block additional
def changeBlockAdditional(numTabs) :
    click(parametersReference)
    # place cursor in block movement checkbox
    for x in range(0, numTabs) :
        type(Key.TAB)
    # Change current value
    type(Key.SPACE)

# netedit wait question
def waitQuestion(NEProcess, answer) :
    try:
        wait(neteditResourceQuestion, 5)
    except:
        NEProcess.kill()
        sys.exit("Killed netedit process. 'question.png' not found")
    #Answer can be "y" or "n"
    type(answer, Key.ALT)
    
# netedit quit
def neteditQuit(mustBeSaved, save) :
    # quit
    type("q", Key.CTRL)

    # Check if net must be saved
    if(mustBeSaved == True) :
        if(save == True) :
            waitQuestion(neteditProcess, "y")
        else :
            waitQuestion(neteditProcess, "n")
    

# netedit save additionals
def neteditSaveAdditionals(match, neteditLoadedAtStart = False) :
    # first move cursor to ensure no menu item is highlighted
    hover(match.getTarget().offset(0, -120))
    
    # open file menu
    type("f", Key.ALT)
    
    # select option "save additionals" depending of neteditLoadedAtStart
    if(neteditLoadedAtStart == True) :
        for x in range(0, 13) :
            type(Key.DOWN)
    else :
        for x in range(0, 12) :
            type(Key.DOWN)
    type(Key.ENTER)
