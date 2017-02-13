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

# set inspect mode
type("i")

# inspect first crossing
click(match.getTarget().offset(250, 225))

# obtain parameters reference
parametersReference = netedit.getParametersReference(match)

# try to change ID (cannot be possible)
netedit.modifyAttribute(parametersReference, 0, "dummyID")

# Change Edges adding a new edge
netedit.modifyAttribute(parametersReference, 1, "3 7 1")

# rebuild network
netedit.rebuildNetwork()

# Change Edges with the same edges as another crossing (Duplicate
# crossings aren't allow)
netedit.modifyAttribute(parametersReference, 1, "4 8")

# rebuild network
netedit.rebuildNetwork()

# Change Edges to a single edge
netedit.modifyAttribute(parametersReference, 1, "3")

# rebuild network
netedit.rebuildNetwork()

# Change priority
netedit.modifyBoolAttribute(parametersReference, 2)

# Change width with a non valid value 1
netedit.modifyAttribute(parametersReference, 3, "dummyWidth")

# Change width with a non valid value 2
netedit.modifyAttribute(parametersReference, 3, "-3")

# Change width with a valid value
netedit.modifyAttribute(parametersReference, 3, "4")

# rebuild network
netedit.rebuildNetwork()

# Check undos
netedit.undo(match, 3)

# rebuild network
netedit.rebuildNetwork()

# Check redos
netedit.redo(match, 3)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
