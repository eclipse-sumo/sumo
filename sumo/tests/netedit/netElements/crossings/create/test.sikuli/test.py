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

# set crossing mode
type("r")

# select central node
click(match.getTarget().offset(325, 225))

# select two left edges and create crossing in edges 3 and 7
click(match.getTarget().offset(150, 200))
click(match.getTarget().offset(150, 250))
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create manual crossing
netedit.modifyCrossingEdges(match, "1 5")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# try to create again the same crossing (cannot be possible, show warning
# instead)
netedit.modifyCrossingEdges(match, "1 5")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create single crossing (fail)
netedit.modifyCrossingEdges(match, "4")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create split crossing
netedit.modifyCrossingEdges(match, "4")
netedit.createCrossing(match)
netedit.modifyCrossingEdges(match, "8")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create manual crossing with different priority and width
netedit.modifyCrossingEdges(match, "6 2")
netedit.modifyCrossingPriority(match)
netedit.modifyCrossingWidth(match, "5")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# Check buttons
click(match.getTarget().offset(150, 200))
click(match.getTarget().offset(150, 250))
netedit.invertCrossings(match)
netedit.clearCrossings(match)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
