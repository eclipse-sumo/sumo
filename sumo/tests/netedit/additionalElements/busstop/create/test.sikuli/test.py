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

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeAdditional("busStop")

# Add three extra lines
netedit.modifyStoppingPlaceLines(3, 3)

# fill extra lines
netedit.fillStoppingPlaceLines(2, 4)

# remove last line (line 4)
netedit.modifyStoppingPlaceLines(7, 1)

# create busStop in mode "reference left"
netedit.leftClick(match, 250, 250)

# change reference to right
netedit.modifyAdditionalDefaultValue(8, "reference right")

# create busStop in mode "reference right"
netedit.leftClick(match, 240, 250)

# change reference to center
netedit.modifyAdditionalDefaultValue(8, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(match, 425, 250)

# return to mode "reference left"
netedit.modifyAdditionalDefaultValue(8, "reference left")

# Change length
netedit.modifyAdditionalDefaultValue(10, "30")

# try to create a busStop in mode "reference left" (Warning)
netedit.leftClick(match, 500, 250)

# change reference to right
netedit.modifyAdditionalDefaultValue(8, "reference right")

# try busStop in mode "reference right" (Warning)
netedit.leftClick(match, 110, 250)

# enable force position
netedit.modifyAdditionalDefaultBoolValue(11)

# change reference to "reference left"
netedit.modifyAdditionalDefaultValue(8, "reference left")

# create a busStop in mode "reference left" forcing poisition
netedit.leftClick(match, 500, 250)

# change reference to "reference right"
netedit.modifyAdditionalDefaultValue(8, "reference right")

# create a busStop in mode "reference right" forcing position
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
