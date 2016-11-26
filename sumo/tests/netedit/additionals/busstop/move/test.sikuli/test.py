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

# create busstop in mode "reference center"
click(match.getTarget().offset(300, 300))

# center edge
rightClick(match.getTarget().offset(305, 305))
click(match.getTarget().offset(330, 330))

# change to move mode
type("m")

# change mouse move delay
Settings.MoveMouseDelay = 0.5

# move bus stop to left
dragDrop(match.getTarget().offset(300, 235), match.getTarget().offset(200, 235))

# move back
dragDrop(match.getTarget().offset(200, 235), match.getTarget().offset(300, 235))

# move bus stop to right
dragDrop(match.getTarget().offset(300, 235), match.getTarget().offset(400, 235))

# move back
dragDrop(match.getTarget().offset(400, 235), match.getTarget().offset(300, 235))

# move bus stop to left overpassing lane
dragDrop(match.getTarget().offset(300, 235), match.getTarget().offset(50, 235))

# move back
dragDrop(match.getTarget().offset(100, 235), match.getTarget().offset(300, 235))

# move bus stop to right overpassing lane
dragDrop(match.getTarget().offset(300, 235), match.getTarget().offset(550, 235))

# move back
dragDrop(match.getTarget().offset(500, 235), match.getTarget().offset(300, 235))

# set back mouse
Settings.MoveMouseDelay = 0.1

# Check undos and redos
neteditUndo(neteditProcess, match, 10)
neteditRedo(neteditProcess, match, 10)

# save additionals
neteditSaveAdditionals(match)

# quit netedit without saving
neteditQuit(True, True)

