# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit waiting 2 seconds to reference (will not be found)
neteditProcess = netedit.setupAndStart(neteditTestRoot, False, True, 2)

# quit netedit
netedit.quit(neteditProcess, False, False)
