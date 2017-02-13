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

# create E3 1
netedit.leftClick(match, 100, 100)

# create E3 2
netedit.leftClick(match, 200, 100)

# select Exit detector
netedit.changeAdditional(comboboxAdditional, -3)

# Create Exit detector for E3 1
netedit.selectChild(comboboxAdditional, 4, 2)
netedit.leftClick(match, 75, 250)

# Create Exit detectors for E3 2
netedit.selectChild(comboboxAdditional, 4, 1)
netedit.leftClick(match, 250, 250)

# Change to delete
netedit.deleteMode()

# delete created E3 1
netedit.leftClick(match, 100, 100)

# delete Exit detector of E3 2 (E3 will not be removed)
netedit.leftClick(match, 250, 250)

# delete loaded E3 1
netedit.leftClick(match, 400, 100)

# delete Exit detector of loaded E3 2 (E3 will not be removed)
netedit.leftClick(match, 500, 250)

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
