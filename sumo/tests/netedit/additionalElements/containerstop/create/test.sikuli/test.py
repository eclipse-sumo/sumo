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

# select containerStop
netedit.changeAdditional(comboboxAdditional, 3)

# Add three extra lines
netedit.modifyNumberOfStoppingPlaceLines(comboboxAdditional, 3, 3)

# fill extra lines
netedit.addStoppingPlaceLines(comboboxAdditional, 2, 4)

# remove last line (line 4)
netedit.modifyNumberOfStoppingPlaceLines(comboboxAdditional, 7, 1)

# create containerStop in mode "reference left"
netedit.leftClick(match, 250, 250)

# change reference to right
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 1)

# create containerStop in mode "reference right"
netedit.leftClick(match, 240, 250)

# change reference to center
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 2)

# create containerStop in mode "reference center"
netedit.leftClick(match, 425, 250)

# return to mode "reference left"
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 0)

# Change length
netedit.modifyStoppingPlaceLength(comboboxAdditional, 10, "30")

# try to create a containerStop in mode "reference left" (Warning)
netedit.leftClick(match, 500, 250)

# change reference to right
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 1)

# try containerStop in mode "reference right" (Warning)
netedit.leftClick(match, 110, 250)

# enable force position
netedit.modifyStoppingPlaceForcePosition(comboboxAdditional, 11)

# change reference to "reference left"
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 0)

# create a containerStop in mode "reference left" forcing poisition
netedit.leftClick(match, 500, 250)

# change reference to "reference right"
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 1)

# create a containerStop in mode "reference right" forcing position
netedit.leftClick(match, 110, 250)

# Check undo redo
netedit.undo(match, 6)
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
