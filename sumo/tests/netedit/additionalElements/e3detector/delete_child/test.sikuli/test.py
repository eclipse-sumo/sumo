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

# apply zoom
netedit.zoomIn(match.getTarget().offset(325, 200), 10)

# Change to create additional
netedit.additionalMode()

# obtain match for comboboxAdditional
comboboxAdditional = netedit.getComboBoxAdditional(match)

# select E3
netedit.changeAdditional(comboboxAdditional, 8)

# create E3
netedit.leftClick(match, 100, 100)

# select entry detector
netedit.changeAdditional(comboboxAdditional, -4)

# Create Entry detector for E3
netedit.selectChild(comboboxAdditional, 4, 3)
netedit.leftClick(match, 100, 250)

# select Exit detector
netedit.changeAdditional(comboboxAdditional, 1)

# Create Exit detector for E3
netedit.selectChild(comboboxAdditional, 4, 3)
netedit.leftClick(match, 250, 250)

# Undo creation of E3, Entry and Exit
netedit.leftClick(match, 0, 0)
netedit.undo(match, 3)

# Change to create additional
netedit.additionalMode()

# select E3
netedit.changeAdditional(comboboxAdditional, 3)

# create E3
netedit.leftClick(match, 100, 100)

# select entry detector
netedit.changeAdditional(comboboxAdditional, -4)

# Create Entry detector for E3
netedit.selectChild(comboboxAdditional, 4, 3)
netedit.leftClick(match, 100, 250)

# select Exit detector
netedit.changeAdditional(comboboxAdditional, 1)

# Create Exit detector for E3
netedit.selectChild(comboboxAdditional, 4, 3)
netedit.leftClick(match, 250, 250)

# Undo creation of Entry and Exit
netedit.leftClick(match, 0, 0)
netedit.undo(match, 2)

# Change to create additional
netedit.additionalMode()

# select entry detector
netedit.changeAdditional(comboboxAdditional, -1)

# Create Entry detector for E3
netedit.selectChild(comboboxAdditional, 4, 3)
netedit.leftClick(match, 100, 250)

# select Exit detector
netedit.changeAdditional(comboboxAdditional, 1)

# Create Exit detector for E3
netedit.selectChild(comboboxAdditional, 4, 3)
netedit.leftClick(match, 250, 250)

# Change to delete
netedit.deleteMode()

# Delete the four Entry/exits
netedit.leftClick(match, 100, 250)
netedit.leftClick(match, 250, 250)
netedit.leftClick(match, 450, 250)
netedit.leftClick(match, 600, 250)

# Check undo redo
netedit.undo(match, 7)
netedit.redo(match, 7)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
