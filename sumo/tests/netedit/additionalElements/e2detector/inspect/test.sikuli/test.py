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

# select E2
netedit.changeAdditional(comboboxAdditional, 7)

# create E2 1
netedit.leftClick(match, 250, 150)

# create E2 2
netedit.leftClick(match, 450, 150)

# Change to inspect mode
netedit.inspectMode()

# inspect first E2
netedit.leftClick(match, 260, 150)

# obtain parameters reference
parametersReference = netedit.getParametersReference(match)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(parametersReference, 0, "e2Detector_gneE2_1_1")

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

# Change parameter 3 with a non valid value
netedit.modifyAttribute(parametersReference, 3, "dummyLength")

# Change parameter 3 with a non valid value
netedit.modifyAttribute(parametersReference, 3, "-5")

# Change parameter 3 with a valid value
netedit.modifyAttribute(parametersReference, 3, "5")

# Change parameter 4 with a non valid value (non numeral)
netedit.modifyAttribute(parametersReference, 4, "dummyFrequency")

# Change parameter 4 with a non valid value (negative)
netedit.modifyAttribute(parametersReference, 4, "-100")

# Change parameter 4 with a valid value
netedit.modifyAttribute(parametersReference, 4, "120")

# Change parameter 5 with an empty value
netedit.modifyAttribute(parametersReference, 5, "")

# Change parameter 5 with a duplicated value
netedit.modifyAttribute(parametersReference, 5, "E2Detector_gneE2_1_1.txt")

# Change parameter 5 with a valid value
netedit.modifyAttribute(parametersReference, 5, "myOwnOutput.txt")

# Change boolean parameter 6
netedit.modifyBoolAttribute(parametersReference, 6)

# Change parameter 7 with a non valid value
netedit.modifyAttribute(parametersReference, 7, "dummyTimeTreshold")

# Change parameter 7 with a non valid value
netedit.modifyAttribute(parametersReference, 7, "-5")

# Change parameter 7 with a valid value
netedit.modifyAttribute(parametersReference, 7, "4")

# Change parameter 8 with a non valid value
netedit.modifyAttribute(parametersReference, 8, "dummySpeedTreshold")

# Change parameter 8 with a non valid value
netedit.modifyAttribute(parametersReference, 8, "-12.1")

# Change parameter 8 with a valid value
netedit.modifyAttribute(parametersReference, 8, "6.3")

# Change parameter 9 with a non valid value
netedit.modifyAttribute(parametersReference, 9, "dummyJamThreshold")

# Change parameter 9 with a non valid value
netedit.modifyAttribute(parametersReference, 9, "-8")

# Change parameter 9 with a valid value
netedit.modifyAttribute(parametersReference, 9, "12.5")

# go to a empty area
click(match)

# Check undos and redos
netedit.undo(match, 23)
netedit.redo(match, 23)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
