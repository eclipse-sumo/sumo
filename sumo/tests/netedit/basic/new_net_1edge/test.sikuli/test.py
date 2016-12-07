# Import libraries
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
_, match = netedit.setupAndStart(neteditTestRoot, True)

# Focus netedit window
click(match.getTarget().offset(0, -105))

# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# save network and quit
type("s", Key.CTRL)
type("q", Key.CTRL)
