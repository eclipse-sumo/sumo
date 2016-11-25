# import common functions for netedit tests
import os
execfile(os.environ.get('SUMO_HOME', '.') + "/tests/netedit/neteditTestFunctions.py")

# Open netedit
neteditProcess = subprocess.Popen([neteditApp,
                                   '--gui-testing',
                                   '--window-size', '700,500',
                                   '--new',
                                   '--additionals-output', textTestSandBox + "/additionals.xml"],
                                   env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# Wait to netedit reference
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

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to create additional
type("a")

# go to additionalsComboBox
click(additionalsComboBox)

# by default, additional is busstop, then isn't needed to select "busstop"

# Add three extra lines
for x in range(0, 3):
	type(Key.TAB)
	
for x in range(0, 3):
	type(Key.SPACE)
	
#add extra lines
click(additionalsComboBox)

for x in range(0, 2):
	type(Key.TAB)
	
paste("Line1")
type(Key.TAB)
paste("Line2")
type(Key.TAB)
paste("Line3")
type(Key.TAB)
paste("Line4")

# remove last line (line 4)
click(additionalsComboBox)

for x in range(0, 7):
	type(Key.TAB)
	
type(Key.SPACE)

# create busstop in mode "reference left"
click(match.getTarget().offset(450, 300))

# change reference to right
modifyStoppingPlaceReference(parametersReference, 8, 1)

# create busstop in mode "reference right"
click(match.getTarget().offset(300, 300))

# change reference to center
modifyStoppingPlaceReference(parametersReference, 8, 2)

# create busstop in mode "reference center"
click(match.getTarget().offset(350, 300))

# return to mode "reference left"
modifyStoppingPlaceReference(parametersReference, 8, 0)

# Change length
click(additionalsComboBox)

for x in range(0, 10):
	type(Key.TAB)
	
paste("30")

# try to create a busstop (Warning)
click(match.getTarget().offset(500, 300))

# change reference to right
modifyStoppingPlaceReference(parametersReference, 8, 1)

# try busstop in mode "reference right" (Warning)
click(match.getTarget().offset(250, 300))

# return to mode "reference left"
modifyStoppingPlaceReference(parametersReference, 8, 0)

# enable force position
click(additionalsComboBox)

for x in range(0, 11):
	type(Key.TAB)
	
type(Key.SPACE)

# create a busstop forcing position
click(match.getTarget().offset(500, 300))

# change reference to right
modifyStoppingPlaceReference(parametersReference, 8, 1)

# create a busstop forcing position
click(match.getTarget().offset(250, 300))

# Check undo redo
neteditUndo(neteditProcess, match, 6)
neteditRedo(neteditProcess, match, 6)

# save additionals
neteditSaveAdditionals(match)

# quit netedit without saving
neteditQuit(True, True)