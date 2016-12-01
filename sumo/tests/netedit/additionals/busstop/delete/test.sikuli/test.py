# import common functions for netedit tests
import os
execfile(os.environ.get('SUMO_HOME', '.') + "/tests/netedit/neteditTestFunctions.py")

# Open netedit
neteditProcess = openNetedit(True)

# Wait to netedit reference
match = getNeteditMatch(neteditProcess)

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

# create busstop in mode "reference left"
click(match.getTarget().offset(450, 300))

# Change to delete
type("d")

# delete created busstop
click(match.getTarget().offset(460, 315))

# Check undo redo
neteditUndo(neteditProcess, match, 3)
neteditRedo(neteditProcess, match, 3)

# save additionals
neteditSaveAdditionals(match)

# quit netedit without saving
neteditQuit(neteditProcess, False, True)
