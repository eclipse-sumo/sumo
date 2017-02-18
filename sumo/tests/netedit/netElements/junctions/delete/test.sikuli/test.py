# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# Focus netedit window
netedit.leftClick(match, 0, -105)

# rebuild network
netedit.rebuildNetwork()

# Change to delete mode
netedit.deleteMode()

# remove one way edge
netedit.leftClick(match, 50, 50)

# remove two way edges
netedit.leftClick(match, 260, 50)

# remove two way edges
netedit.leftClick(match, 500, 50)

# remove square
netedit.leftClick(match, 60, 160)
netedit.leftClick(match, 150, 280)

# remove circular road
netedit.leftClick(match, 450, 270)

# rebuild network
netedit.rebuildNetwork()

# Check undo
netedit.undo(match, 6)

# rebuild network
netedit.rebuildNetwork()

# Check redo
netedit.redo(match, 6)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
