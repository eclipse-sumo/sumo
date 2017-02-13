# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, True)

# Focus netedit window
netedit.leftClick(match, 0, -105)

# Change to create mode
type("e")

# Create two nodes
netedit.leftClick(match, 100, 300)
netedit.leftClick(match, 500, 300)

# Check undo and redo
netedit.undo(match, 1)
netedit.redo(match, 1)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
