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

# go to additional mode
netedit.additionalMode()

# select E3
netedit.changeAdditional("e3Detector")

# create E3 with default parameters
netedit.leftClick(match, 275, 100)

# select Exit detector
netedit.changeAdditional("detExit")

# try to create Exit without select child
netedit.leftClick(match, 50, 250)

# Create four Exit detectors
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 50, 250)
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 200, 250)
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 350, 250)
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 500, 250)

# Check undo redo
netedit.undo(match, 5)
netedit.redo(match, 5)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
