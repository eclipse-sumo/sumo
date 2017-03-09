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

# select E1
netedit.changeAdditional("e1Detector")

# create E1
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move E1 to right
netedit.moveElement(match, 120, 250, 250, 250)

# go to inspect mode
netedit.inspectMode()

# inspect E1
netedit.leftClick(match, 350, 250)

# block additional
netedit.modifyBoolAttribute(6)

# change to move mode
netedit.moveMode()

# try to move E1 to right (must be blocked)
netedit.moveElement(match, 250, 250, 350, 250)

# go to inspect mode
netedit.inspectMode()

# inspect E1
netedit.leftClick(match, 350, 250)

# unblock additional
netedit.modifyBoolAttribute(6)

# change to move mode
netedit.moveMode()

# move E1 to right (must be allowed)
netedit.moveElement(match, 250, 250, 350, 250)

# Check undos and redos
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
