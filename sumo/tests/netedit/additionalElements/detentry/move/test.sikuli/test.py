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
netedit.leftClick(match, 250, 100)

# select entry detector
netedit.changeAdditional(comboboxAdditional, -4)

# Create Entry detector
netedit.selectChild(comboboxAdditional, 4, 1)
netedit.leftClick(match, 100, 250)

# apply zoom out
netedit.zoomOut(match.getTarget().offset(325, 200), 10)

# change to move mode
netedit.moveMode()

# move Entry to left
netedit.moveElement(match, 120, 200, 50, 200)

# move back
netedit.moveElement(match, 50, 200, 120, 200)

# move Entry to right
netedit.moveElement(match, 120, 200, 250, 200)

# move back
netedit.moveElement(match, 250, 200, 120, 200)

# move Entry to left overpassing lane
netedit.moveElement(match, 120, 200, -150, 200)

# move back
netedit.moveElement(match, -80, 200, 120, 200)

# move Entry to right overpassing lane
netedit.moveElement(match, 120, 200, 580, 200)

# move back to another different position of initial
netedit.moveElement(match, 520, 200, 300, 200)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
