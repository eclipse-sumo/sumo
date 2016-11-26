# import common functions for netedit tests
import os
execfile(os.environ.get('SUMO_HOME', '.') + "/tests/netedit/neteditTestFunctions.py")

# Open netedit
neteditProcess = openNetedit(True)

# Wait to netedit reference
match = getNeteditMatch(neteditProcess)

# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(300, 300))

# Change to create additional
type("a")

# go to additionalsComboBox
click(additionalsComboBox)

# by default, additional is busstop, then isn't needed to select "busstop"

# obtain match for additionalsComboBox
additionalsComboBox = match.getTarget().offset(-75, 50)

# obtain reference for parameters (In this case, is the same as the additionalsComboBox)
parametersReference = additionalsComboBox

# change reference to center
modifyStoppingPlaceReference(parametersReference, 6, 2)

# block additional
changeBlockAdditional(10)

# create busstop in mode "reference center"
click(match.getTarget().offset(300, 300))

# center edge
rightClick(match.getTarget().offset(305, 305))
click(match.getTarget().offset(330, 330))

# change to move mode
type("m")

# try to move bus stop (must be blocked)
Settings.MoveMouseDelay = 0.5
dragDrop(match.getTarget().offset(300, 235), match.getTarget().offset(200, 235))
Settings.MoveMouseDelay = 0.1

# Check undo redo
neteditUndo(neteditProcess, match, 2)
neteditRedo(neteditProcess, match, 2)

# try to move again bus stop (must be blocked)
Settings.MoveMouseDelay = 0.5
dragDrop(match.getTarget().offset(300, 235), match.getTarget().offset(200, 235))
Settings.MoveMouseDelay = 0.1

# save additionals
neteditSaveAdditionals(match)

# quit netedit without saving
neteditQuit(True, True)