# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, False)

# Change to create additional
netedit.additionalMode()

# obtain match for comboboxAdditional
comboboxAdditional = netedit.getComboBoxAdditional(match)

# select busStop
netedit.changeAdditional(comboboxAdditional, 0)

# change reference to center
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 2)

# create busStop in mode "reference center"
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move busStop to left
netedit.moveElement(match, 150, 275, 50, 275)

# move back
netedit.moveElement(match, 50, 275, 150, 275)

# move busStop to right
netedit.moveElement(match, 150, 275, 250, 275)

# move back
netedit.moveElement(match, 250, 275, 150, 275)

# move busStop to left overpassing lane
netedit.moveElement(match, 150, 275, -100, 275)

# move back
netedit.moveElement(match, 0, 275, 150, 275)

# move busStop to right overpassing lane
netedit.moveElement(match, 150, 275, 550, 275)

# move back to another different position of initial
netedit.moveElement(match, 500, 275, 300, 275)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
