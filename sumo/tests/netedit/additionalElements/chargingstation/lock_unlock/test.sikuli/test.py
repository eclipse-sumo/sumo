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

# select chargingStation
netedit.changeAdditional("chargingStation")

# change reference to center
netedit.modifyAdditionalDefaultValue(7, "reference center")

# create chargingStation in mode "reference center"
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move chargingStation to right
netedit.moveElement(match, 150, 250, 250, 250)

# go to inspect mode
netedit.inspectMode()

# inspect chargingStation
netedit.leftClick(match, 300, 250)

# block additional
netedit.modifyBoolAttribute(8)

# change to move mode
netedit.moveMode()

# try to move chargingStation to right (must be blocked)
netedit.moveElement(match, 250, 250, 350, 250)

# go to inspect mode
netedit.inspectMode()

# inspect chargingStation
netedit.leftClick(match, 300, 250)

# unblock additional
netedit.modifyBoolAttribute(8)

# change to move mode
netedit.moveMode()

# move chargingStation to right (must be allowed)
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
