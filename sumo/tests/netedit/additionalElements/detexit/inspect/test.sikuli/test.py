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

# apply zoom
netedit.zoomIn(match.getTarget().offset(325, 100), 10)

# Change to create additional
netedit.additionalMode()

# obtain match for comboboxAdditional
comboboxAdditional = netedit.getComboBoxAdditional(match)

# select E3
netedit.changeAdditional(comboboxAdditional, 8)

# create E3
netedit.leftClick(match, 250, 400)

# select Exit detector
netedit.changeAdditional(comboboxAdditional, -3)

# Create Exit detector E3
netedit.selectChild(comboboxAdditional, 4, 0)
netedit.leftClick(match, 200, 180)

# Change to inspect mode
netedit.inspectMode()

# inspect Exit
netedit.leftClick(match, 250, 180)

# obtain parameters reference
parametersReference = netedit.getParametersReference(match)

# Change parameter 0 with a non valid value (dummy Lane)
netedit.modifyAttribute(parametersReference, 0, "dummyLane")

# Change parameter 0 with a non valid value (Empty lane)
netedit.modifyAttribute(parametersReference, 0, "")

# Change parameter 0 with a valid value (other lane)
netedit.modifyAttribute(parametersReference, 0, "gneE3_0")

# Change parameter 1 with a non valid value (dummy position X)
netedit.modifyAttribute(parametersReference, 1, "dummy position")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(parametersReference, 1, "")

# Change parameter 1 with a valid value (different position X)
netedit.modifyAttribute(parametersReference, 1, "25")

# go to a empty area
netedit.leftClick(match, 0, 0)

# Check undos and redos
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
