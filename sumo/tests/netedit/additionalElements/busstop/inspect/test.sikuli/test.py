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

# Change to create additional
netedit.additionalMode()

# obtain match for comboboxAdditional
comboboxAdditional = netedit.getComboBoxAdditional(match)

# select busStop
netedit.changeAdditional(comboboxAdditional, 0)

# change reference to center
netedit.modifyStoppingPlaceReference(comboboxAdditional, 8, 2)

# create busStop 1 in mode "reference center"
netedit.leftClick(match, 250, 150)

# create busStop 2 in mode "reference center"
netedit.leftClick(match, 450, 150)

# Change to inspect mode
netedit.inspectMode()

# inspect first busStop
netedit.leftClick(match, 250, 170)

# obtain parameters reference
parametersReference = netedit.getParametersReference(match)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(parametersReference, 0, "busStop_gneE2_1_1")

# Change parameter 0 with a valid value
netedit.modifyAttribute(parametersReference, 0, "correct ID")

# Change parameter 1 with a non valid value (dummy lane)
netedit.modifyAttribute(parametersReference, 1, "dummy lane")

# Change parameter 1 with a valid value (different edge)
netedit.modifyAttribute(parametersReference, 1, "gneE0_0")

# Change parameter 1 with a valid value (original edge, same lane)
netedit.modifyAttribute(parametersReference, 1, "gneE2_1")

# Change parameter 1 with a valid value (original edge, different lane)
netedit.modifyAttribute(parametersReference, 1, "gneE2_0")

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(parametersReference, 2, "-5")

# Change parameter 2 with a non valid value (> endPos)
netedit.modifyAttribute(parametersReference, 2, "400")

# Change parameter 2 with a valid value
netedit.modifyAttribute(parametersReference, 2, "20")

# Change parameter 3 with a non valid value (out of range, and not accepted)
netedit.modifyAttribute(parametersReference, 3, "3000")

# Answer "no" to the answer dialog
netedit.waitQuestion("n")

# Change parameter 3 with a valid value (out of range, but adapted to the
# end of lane)
netedit.modifyAttribute(parametersReference, 3, "3000")

# Answer "yes" to the answer dialog
netedit.waitQuestion("y")

# Change parameter 3 with a non valid value (<startPos)
netedit.modifyAttribute(parametersReference, 3, "10")

# Change parameter 3 with a valid value
netedit.modifyAttribute(parametersReference, 3, "30")

# Change parameter 4 with a non valid value (throw warning)
netedit.modifyAttribute(parametersReference, 4, "line1, line2")

# Change parameter 4 with a valid value
netedit.modifyAttribute(parametersReference, 4, "line1 line2")

# go to a empty area
click(match)

# Check undos and redos
netedit.undo(match, 13)
netedit.redo(match, 13)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
