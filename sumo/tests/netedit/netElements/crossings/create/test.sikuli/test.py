# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, True)

# Change to create mode
type("e")

# Create five nodes
click(match.getTarget().offset(100, 200))
click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(100, 200))

click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(300, 200))
click(match.getTarget().offset(300, 200))
click(match.getTarget().offset(200, 200))

click(match.getTarget().offset(200, 100))
click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(200, 100))

click(match.getTarget().offset(200, 300))
click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(200, 200))
click(match.getTarget().offset(200, 300))

# zoom in central node
netedit.zoomIn(match.getTarget().offset(200, 200), 10)

# set crossing mode
type("r")

# select central node
click(match.getTarget().offset(250, 200))

# select two left edges and create crossing
click(match.getTarget().offset(200, 180))
click(match.getTarget().offset(200, 210))
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create manual crossing
netedit.modifyCrossingEdges(match, "gneE2 gneE3")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create again the same crossing
netedit.modifyCrossingEdges(match, "gneE2 gneE3")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create single crossing
netedit.modifyCrossingEdges(match, "gneE4")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# create manual crossing with different priority and width
netedit.modifyCrossingEdges(match, "gneE6 gneE7")
netedit.modifyCrossingPriority(match)
netedit.modifyCrossingWidth(match, "5")
netedit.createCrossing(match)
netedit.rebuildNetwork()

# Check buttons
click(match.getTarget().offset(200, 180))
click(match.getTarget().offset(200, 210))
netedit.invertCrossings(match)
netedit.clearCrossings(match)

# save newtork
netedit.saveNetwork()

# quit netedit 
netedit.quit(neteditProcess, False, False)