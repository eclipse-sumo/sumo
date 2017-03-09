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

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(match, 250, 250)

# create E3 2 (for check duplicated ID)
netedit.leftClick(match, 450, 250)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detector E3 (for saving)
netedit.selectAdditionalChild(4, 0)
netedit.leftClick(match, 100, 180)
netedit.selectAdditionalChild(4, 1)
netedit.leftClick(match, 400, 180)

# go to inspect mode
netedit.inspectMode()

# inspect first E3
netedit.leftClick(match, 250, 250)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "e3Detector_1")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correct ID")

# Change parameter 1 with a non valid value (dummy position X)
netedit.modifyAttribute(1, "dummy position X")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(1, "")

# Change parameter 1 with a valid value (different position X)
netedit.modifyAttribute(1, "25")

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummy position Y")

# Change parameter 1 with a non valid value (empty)
netedit.modifyAttribute(2, "")

# Change parameter 2 with a non valid value (different position Y)
netedit.modifyAttribute(2, "25")

# Change parameter 3 with a non valid value (non numeral)
netedit.modifyAttribute(3, "dummyFrequency")

# Change parameter 3 with a non valid value (negative)
netedit.modifyAttribute(3, "-100")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "120")

# Change parameter 4 with an empty value
netedit.modifyAttribute(4, "")

# Change parameter 4 with a duplicated value
netedit.modifyAttribute(4, "e3Detector_1.txt")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "myOwnOutput.txt")

# Change parameter 5 with a non valid value (dummy)
netedit.modifyAttribute(5, "dummyTimeTreshold")

# Change parameter 5 with a non valid value (negative)
netedit.modifyAttribute(5, "-5")

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "4")

# Change parameter 6 with a non valid value (dummy)
netedit.modifyAttribute(6, "dummySpeedTreshold")

# Change parameter 6 with a non valid value (negative)
netedit.modifyAttribute(6, "-12.1")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "6.3")

# go to a empty area
netedit.leftClick(match, 0, 0)

# Check undos and redos
netedit.undo(match, 21)
netedit.redo(match, 21)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
