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

# select E3
netedit.changeAdditional("e3Detector")

# create E3
netedit.leftClick(match, 250, 250)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector E3 (for saving)
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 400, 250)

# change to move mode
netedit.moveMode()

# move E3 to up
netedit.moveElement(match, 120, 250, 120, 50)

# go to inspect mode
netedit.inspectMode()

# inspect E3
netedit.leftClick(match, 250, 100)

# block additional
netedit.modifyBoolAttribute(7)

# change to move mode
netedit.moveMode()

# try to move E3 to right (must be blocked)
netedit.moveElement(match, 120, 50, 300, 50)

# go to inspect mode
netedit.inspectMode()

# inspect E3
netedit.leftClick(match, 250, 100)

# unblock additional
netedit.modifyBoolAttribute(7)

# change to move mode
netedit.moveMode()

# move E3 to right (must be allowed)
netedit.moveElement(match, 120, 50, 300, 50)

# Check undos and redos
netedit.undo(match, 6)
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
