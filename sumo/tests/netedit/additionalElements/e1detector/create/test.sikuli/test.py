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

# select E1
netedit.changeAdditional(comboboxAdditional, 6)

# create E1 with default parameters
netedit.leftClick(match, 200, 250)

# Change frequency
netedit.modifyAttribute(comboboxAdditional, 2, "150")

# create E1 with different frequency
netedit.leftClick(match, 250, 250)

# Change frequency
netedit.modifyBoolAttribute(comboboxAdditional, 3)

# create E1 with different split by type
netedit.leftClick(match, 300, 250)

# Check undo redo
netedit.undo(match, 3)
netedit.redo(match, 3)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
