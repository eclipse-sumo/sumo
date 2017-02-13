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

# select chargingStation
netedit.changeAdditional(comboboxAdditional, 2)

# change reference to center
netedit.modifyStoppingPlaceReference(comboboxAdditional, 7, 2)

# create chargingStation in mode "reference center"
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move chargingStation to left
netedit.moveElement(match, 150, 250, 50, 250)

# move back
netedit.moveElement(match, 50, 250, 150, 250)

# move chargingStation to right
netedit.moveElement(match, 150, 250, 250, 250)

# move back
netedit.moveElement(match, 250, 250, 150, 250)

# move chargingStation to left overpassing lane
netedit.moveElement(match, 150, 250, -100, 250)

# move back
netedit.moveElement(match, 0, 250, 150, 250)

# move chargingStation to right overpassing lane
netedit.moveElement(match, 150, 250, 550, 250)

# move back to another different position of initial
netedit.moveElement(match, 500, 250, 300, 250)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
