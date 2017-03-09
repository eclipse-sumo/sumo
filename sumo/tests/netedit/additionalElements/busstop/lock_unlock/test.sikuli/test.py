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

# select busStop
netedit.changeAdditional("busStop")

# change reference to center
netedit.modifyAdditionalDefaultValue(6, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move busStop to right
netedit.moveElement(match, 150, 275, 250, 275)

# go to inspect mode
netedit.inspectMode()

# inspect busStop
netedit.leftClick(match, 300, 250)

# block additional
netedit.modifyBoolAttribute(5)

# change to move mode
netedit.moveMode()

# try to move busStop to right (must be blocked)
netedit.moveElement(match, 250, 275, 350, 275)

# go to inspect mode
netedit.inspectMode()

# inspect busStop
netedit.leftClick(match, 300, 250)

# unblock additional
netedit.modifyBoolAttribute(5)

# change to move mode
netedit.moveMode()

# move busStop to right (must be allowed)
netedit.moveElement(match, 250, 275, 350, 275)

# Check undos and redos
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
