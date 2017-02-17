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
netedit.inspectMode()

# Change to inspect mode
netedit.inspectMode()

# obtain parameters reference
parametersReference = netedit.getParametersReference(match)

# inspect first node
netedit.leftClick(match, 50, 50)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 1)

# inspect second node
netedit.leftClick(match, 150, 50)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 2)

# inspect third node
netedit.leftClick(match, 265, 50)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 3)

# inspect four node
netedit.leftClick(match, 380, 50)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 4)

# inspect five node
netedit.leftClick(match, 495, 50)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 5)

# inspect five node
netedit.leftClick(match, 610, 50)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 6)

# inspect six node
netedit.leftClick(match, 50, 175)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 7)

# inspect seven node
netedit.leftClick(match, 150, 175)

# change type of junction
netedit.changeTypeOfJunction(parametersReference, 8)












# rebuild network
netedit.rebuildNetwork()

# Check undo
#netedit.undo(match, 6)

# rebuild network
#netedit.rebuildNetwork()

# Check redo
#netedit.redo(match, 6)

# save additionals
#netedit.saveAdditionals()

# save newtork
#netedit.saveNetwork()

# quit netedit
#netedit.quit(neteditProcess, False, False)
