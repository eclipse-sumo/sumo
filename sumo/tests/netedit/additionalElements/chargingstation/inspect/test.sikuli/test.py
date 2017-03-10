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

# select chargingStation
netedit.changeAdditional("chargingStation")

# change reference to center
netedit.modifyAdditionalDefaultValue(7, "reference center")

# create chargingStation 1 in mode "reference center"
netedit.leftClick(match, 250, 150)

# create chargingStation 2 in mode "reference center"
netedit.leftClick(match, 450, 150)

# go to inspect mode
netedit.inspectMode()

# inspect first chargingStation
netedit.leftClick(match, 250, 150)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "chargingStation_gneE2_1_1")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correct ID")

# Change parameter 1 with a non valid value (dummy lane)
netedit.modifyAttribute(1, "dummy lane")

# Change parameter 1 with a valid value (different edge)
netedit.modifyAttribute(1, "gneE0_0")

# Change parameter 1 with a valid value (original edge, same lane)
netedit.modifyAttribute(1, "gneE2_1")

# Change parameter 1 with a valid value (original edge, different lane)
netedit.modifyAttribute(1, "gneE2_0")

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-5")

# Change parameter 2 with a non valid value (> endPos)
netedit.modifyAttribute(2, "400")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "20")

# Change parameter 3 with a non valid value (out of range, and not accepted)
netedit.modifyAttribute(3, "3000")

# Answer "no" to the answer dialog
netedit.waitQuestion("n")

# Change parameter 3 with a valid value (out of range, but adapted to the
# end of lane)
netedit.modifyAttribute(3, "3000")

# Answer "yes" to the answer dialog
netedit.waitQuestion("y")

# Change parameter 3 with a non valid value (<startPos)
netedit.modifyAttribute(3, "10")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "30")

# Change parameter 4 with a non valid value 1
netedit.modifyAttribute(4, "dummyPower")

# Change parameter 4 with a non valid value 2
netedit.modifyAttribute(4, "-100")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "10000")

# Change parameter 5 with a non valid value 1
netedit.modifyAttribute(5, "dummyEfficiency")

# Change parameter 5 with a non valid value 2
netedit.modifyAttribute(5, "-1")

# Change parameter 5 with a non valid value 3
netedit.modifyAttribute(5, "2")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "0.90")

# Change parameter 6 (bool)
netedit.modifyBoolAttribute(6)

# Change parameter 7 with a non valid value 1
netedit.modifyAttribute(7, "dummyChargeDelay")

# Change parameter 7 with a non valid value 2
netedit.modifyAttribute(7, "-10")

# Change parameter 7 with a valid value
netedit.modifyAttribute(7, "3")

# go to a empty area
click(match)

# Check undos and redos
netedit.undo(match, 17)
netedit.redo(match, 17)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
