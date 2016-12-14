import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, True)

# obtain match for additionalsComboBox
additionalsComboBox = match.getTarget().offset(-75, 50)

# Focus netedit window
click(match)

# Change to create mode
type("e")

# Create first two nodes
click(match.getTarget().offset(100, 150))
click(match.getTarget().offset(500, 150))

# Create second two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to inspect
type("i")

# get menu of lane and duplicate it
rightClick(match.getTarget().offset(150, 160))
click(match.getTarget().offset(160, 460))

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

# obtain reference for parameters (In this case, is the same as the additionalsComboBox)
parametersReference = additionalsComboBox

# change reference to center
netedit.modifyStoppingPlaceReference(parametersReference, 6, 2)

# create first busstop in mode "reference center"
click(match.getTarget().offset(400, 150))

# create second busstop in mode "reference center"
click(match.getTarget().offset(400, 300))

# Change to inspect mode
type("i")

# inspect first busstop
click(match.getTarget().offset(400, 170))

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(parametersReference, 0, "busStop_gneE1_0_1")

# Change parameter 0 with a valid value
netedit.modifyAttribute(parametersReference, 0, "correct ID")

# Change parameter 1 with a non valid value (dummy lane)
netedit.modifyAttribute(parametersReference, 1, "dummy lane")

# Change parameter 1 with a valid value (different edge)
netedit.modifyAttribute(parametersReference, 1, "gneE0_0")

# Change parameter 1 with a valid value (same edge, different lane)
netedit.modifyAttribute(parametersReference, 1, "gneE0_1")

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

# Change parameter 3 with a valid value (out of range, but adapted to the end of lane)
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
click(match);

# Check undos and redos
netedit.undo(match, 13)
netedit.redo(match, 13)

# Save additionals
netedit.saveAdditionals(match)

# quit netedit saving net
netedit.quit(neteditProcess, True, True)
