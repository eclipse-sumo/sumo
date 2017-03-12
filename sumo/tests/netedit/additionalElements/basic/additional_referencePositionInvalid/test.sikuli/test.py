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

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeAdditional("busStop")

# set invalid reference (dummy)
netedit.modifyAdditionalDefaultValue(6, "dummy reference")

# try to create busStop with the dummy reference
netedit.leftClick(match, 240, 250)

# set valid reference
netedit.modifyAdditionalDefaultValue(6, "reference right")

# create busStop with the valid reference
netedit.leftClick(match, 300, 250)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
