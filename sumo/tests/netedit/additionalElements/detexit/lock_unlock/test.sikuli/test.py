# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# apply zoom
netedit.setZoom("25", "0", "25")

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3
netedit.leftClick(match, 250, 50)

# select exit detector
netedit.changeAdditional("detExit")

# Create Exit detector
netedit.selectAdditionalChild(6, 0)
netedit.leftClick(match, 100, 200)

# change to move mode
netedit.moveMode()

# move Exit to right
netedit.moveElement(match, 15, 200, 150, 200)

# go to inspect mode
netedit.inspectMode()

# inspect Exit
netedit.leftClick(match, 200, 200)

# block additional
netedit.modifyBoolAttribute(3)

# change to move mode
netedit.moveMode()

# try to move Exit to right (must be blocked)
netedit.moveElement(match, 150, 200, 300, 200)

# go to inspect mode
netedit.inspectMode()

# inspect Exit
netedit.leftClick(match, 200, 200)

# unblock additional
netedit.modifyBoolAttribute(3)

# change to move mode
netedit.moveMode()

# move Exit to right (must be allowed)
netedit.moveElement(match, 150, 200, 300, 200)

# Check undos and redos
netedit.undo(match, 6)
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
