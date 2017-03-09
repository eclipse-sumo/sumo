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

# Rebuild network
netedit.rebuildNetwork()

# zoom in central node
netedit.zoomIn(match.getTarget().offset(325, 225), 10)

# go to delete mode
netedit.deleteMode()

# delete first crossing
netedit.leftClick(match, 250, 225)

# delete second crossing
netedit.leftClick(match, 385, 225)

# check undo redo
netedit.undo(match, 2)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
