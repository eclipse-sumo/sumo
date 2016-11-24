# Import libraries
import os
import sys
import subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
# SUMO Folder
SUMOFolder = os.environ.get('SUMO_HOME', '.')
# Current environment
currentEnvironmentFile = open(SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
# Get path to netEdit app
neteditApp = currentEnvironmentFile.readline().replace("\n", "")
# Get SandBox folder
textTestSandBox = currentEnvironmentFile.readline().replace("\n", "")
# Get resources depending of the current Operating system
currentOS = currentEnvironmentFile.readline().replace("\n", "")
neteditResources = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/"
neteditReference = SUMOFolder + "/tests/netedit/imageResources/reference.png"
currentEnvironmentFile.close()

def neteditUndo(match) :
	type("e", Key.ALT)
	try:
		click(neteditResources + "edit-undo.png")
		click(match)
	except:
		neteditProcess.kill()
		sys.exit("Killed netedit process. 'edit-undo.png' not found")
	
def neteditRedo(match) :
	type("e", Key.ALT)
	try:
		click(neteditResources + "edit-redo.png")
		click(match)
	except:
		neteditProcess.kill()
		sys.exit("Killed netedit process. 'edit-redo.png' not found")

def modifyAttribute(parametersReference, attributeNumber, value) :
	click(parametersReference)
	for x in range(0, attributeNumber) :
		type(Key.TAB)
	type("a", Key.CTRL)
	paste(value)
	type(Key.ENTER)
	
def waitQuestion(answer) :
	try:
		wait(neteditResources + "question.png", 5)
	except:
		neteditProcess.kill()
		sys.exit("Killed netedit process. 'question.png' not found")
	#Answer can be "y" or "n"
	type(answer, Key.ALT)
	
	
def neteditQuit() :
	# quit
	type("q", Key.CTRL)

	# confirm unsafed network
	type("y", Key.ALT)
	type("z", Key.ALT) # work-around misinterpreted keyboard mapping
#****#

# Open netedit
neteditProcess = subprocess.Popen([neteditApp,
                                   '--gui-testing',
                                   '--window-size', '700,500',
                                   '--new',
                                   '--additionals-output', textTestSandBox + "/additionals.xml"],
                                   env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# Wait to netedit and focus
try:
    match = wait(neteditReference, 20)
except:
    neteditProcess.kill()
    sys.exit("Killed netedit process. 'reference.png' not found")

# obtain match for additionalsComboBox
additionalsComboBox = match.getTarget().offset(-75, 50)
	
# Focus netedit window
click(match)
	
# Change to create mode
type("e")

# Create first two nodes
click(match.getTarget().offset(100, 150))
click(match.getTarget().offset(500, 150))

# Create second two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to inspect
type("i")

# get menu of lane and duplicate it
rightClick(match.getTarget().offset(150, 160))
click(match.getTarget().offset(160, 460))

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

# change reference to center
click(additionalsComboBox)
for x in range(0, 6):
	type(Key.TAB)
for x in range(0, 2):
	type(Key.DOWN)
	
# create first busstop in mode "reference center"
click(match.getTarget().offset(400, 150))

# create second busstop in mode "reference center"
click(match.getTarget().offset(400, 300))

# Change to inspect mode
type("i")

# inspect first busstop
click(match.getTarget().offset(400, 170))

# obtain reference for parameters (In this case, is the same as the additionalsComboBox)
parametersReference = additionalsComboBox

# Change parameter 0 with a non valid value (Duplicated ID)
modifyAttribute(parametersReference, 0, "busStop_gneE1_0_1")

# Change parameter 0 with a valid value
modifyAttribute(parametersReference, 0, "correct ID")

# Change parameter 1 with a non valid value (dummy lane)
modifyAttribute(parametersReference, 1, "dummy lane")

# Change parameter 1 with a valid value (different edge)
modifyAttribute(parametersReference, 1, "gneE0_0")

# Change parameter 1 with a valid value (same edge, different lane)
modifyAttribute(parametersReference, 1, "gneE0_1")

# Change parameter 2 with a non valid value (negative)
modifyAttribute(parametersReference, 2, "-5")

# Change parameter 2 with a non valid value (> endPos)
modifyAttribute(parametersReference, 2, "400")

# Change parameter 2 with a valid value
modifyAttribute(parametersReference, 2, "20")

# Change parameter 3 with a non valid value (out of range, and not accepted)
modifyAttribute(parametersReference, 3, "3000")

# Answer "no" to the answer dialog
waitQuestion("n")

# Change parameter 3 with a valid value (out of range, but adapted to the end of lane)
modifyAttribute(parametersReference, 3, "3000")

# Answer "yes" to the answer dialog
waitQuestion("y")

# Change parameter 3 with a non valid value (<startPos)
modifyAttribute(parametersReference, 3, "10")

# Change parameter 3 with a valid value
modifyAttribute(parametersReference, 3, "30")

# Change parameter 4 with a non valid value (throw warning)
modifyAttribute(parametersReference, 4, "line1, line2")

# Change parameter 4 with a valid value
modifyAttribute(parametersReference, 4, "line1 line2")

# go to a empty area
click(match);

# Check undo redo
for x in range(0, 13):
	neteditUndo(match)

for x in range(0, 13):
	neteditRedo(match)

# quit netedit
neteditQuit()