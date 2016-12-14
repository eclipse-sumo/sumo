# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, True)

# obtain match for additionalsComboBox
additionalsComboBox = match.getTarget().offset(-75, 50)

# Focus netedit window
click(match)

# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to create additional
type("a")

# go to additionalsComboBox
click(additionalsComboBox)

# by default, additional is busstop, then isn't needed to select "busstop"

# create busstop in mode "reference left"
click(match.getTarget().offset(450, 300))

# Change to delete
type("d")

# delete created busstop
click(match.getTarget().offset(460, 315))

# Check undo redo
netedit.undo(match, 3)
netedit.redo(match, 3)

# save additionals
netedit.saveAdditionals(match)

# quit netedit without saving
netedit.quit(neteditProcess, False, True)
