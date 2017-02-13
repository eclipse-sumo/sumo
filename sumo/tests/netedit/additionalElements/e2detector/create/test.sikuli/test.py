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

# create E2 with default parameters
netedit.leftClick(match, 125, 250)

# Change lenght
netedit.modifyAttribute(comboboxAdditional, 2, "5")

# create E2 with different lenght
netedit.leftClick(match, 250, 250)

# Change frequency
netedit.modifyAttribute(comboboxAdditional, 3, "120")

# create E2 with different frequency
netedit.leftClick(match, 350, 250)

# Change cont
netedit.modifyBoolAttribute(comboboxAdditional, 4)

# create E2 with different cont
netedit.leftClick(match, 450, 250)

# Change time threshold
netedit.modifyAttribute(comboboxAdditional, 5, "10")

# create E2 with different time threshold
netedit.leftClick(match, 150, 220)

# Change speed threshold
netedit.modifyAttribute(comboboxAdditional, 6, "2.5")

# create E2 with different speed threshold
netedit.leftClick(match, 250, 220)

# Change jam threshold
netedit.modifyAttribute(comboboxAdditional, 7, "15.5")

# create E2 with different jam threshold
netedit.leftClick(match, 350, 220)

# Check undo redo
netedit.undo(match, 7)
netedit.redo(match, 7)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
