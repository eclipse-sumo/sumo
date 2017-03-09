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

# delete junction
netedit.leftClick(match, 300, 250)

# check undo
netedit.undo(match, 1)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
