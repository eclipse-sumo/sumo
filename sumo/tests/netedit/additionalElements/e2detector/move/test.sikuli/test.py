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

# select E2
netedit.changeAdditional(comboboxAdditional, 7)

# create E2
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move E2 to left
netedit.moveElement(match, 120, 250, 50, 250)

# move back
netedit.moveElement(match, 50, 250, 120, 250)

# move E2 to right
netedit.moveElement(match, 120, 250, 250, 250)

# move back
netedit.moveElement(match, 250, 250, 120, 250)

# move E2 to left overpassing lane
netedit.moveElement(match, 120, 250, -150, 250)

# move back
netedit.moveElement(match, -100, 250, 120, 250)

# move E2 to right overpassing lane
netedit.moveElement(match, 120, 250, 580, 250)

# move back to another different position of initial
netedit.moveElement(match, 520, 250, 300, 250)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
