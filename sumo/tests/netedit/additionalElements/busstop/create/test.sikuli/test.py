# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, True)

# Change to create additional
netedit.additionalMode()

# obtain match for comboboxAdditional
comboboxAdditional = netedit.getComboBoxAdditional(match)

# select busstop
netedit.changeAdditional(comboboxAdditional, 0)

# Add three extra lines
netedit.modifyNumberOfStoppingPlaceLines(comboboxAdditional, 3, 3)
    
#add extra lines
netedit.addStoppingPlaceLines(comboboxAdditional, 2, 4)

# remove last line (line 4)
netedit.modifyNumberOfStoppingPlaceLines(comboboxAdditional, 7, 1)

# create busstop in mode "reference left"
click(match.getTarget().offset(250, 250))

# change reference to right
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 1)

# create busstop in mode "reference right"
click(match.getTarget().offset(240, 250))

# change reference to center
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 2)

# create busstop in mode "reference center"
click(match.getTarget().offset(400, 250))

# return to mode "reference left"
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 0)

# Change length
netedit.modifyStoppingPlaceLength(comboboxAdditional, 10, "30")

# try to create a busstop in mode "reference left" (Warning)
click(match.getTarget().offset(500, 250))

# change reference to right
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 1)

# try busstop in mode "reference right" (Warning)
click(match.getTarget().offset(110, 250))

# enable force position
netedit.modifyStoppingPlaceForcePosition(comboboxAdditional, 11)

# change reference to "reference left"
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 0)

# create a busstop in mode "reference left" forcing poisition
click(match.getTarget().offset(500, 250))

# change reference to "reference right"
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 1)

# create a busstop in mode "reference right" forcing position
click(match.getTarget().offset(110, 250))

# Check undo redo
netedit.undo(match, 6)
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit 
netedit.quit(neteditProcess, False, False)