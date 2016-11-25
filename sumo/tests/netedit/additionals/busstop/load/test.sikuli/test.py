# import common functions for netedit tests
import os
execfile(os.environ.get('SUMO_HOME', '.') + "/tests/netedit/neteditTestFunctions.py")

# Open netedit
neteditProcess = openNetedit(False)

# Wait to netedit reference
match = getNeteditMatch(neteditProcess)

# save additionals
neteditSaveAdditionals(match)

#quit without saving
neteditQuit(False, False)

