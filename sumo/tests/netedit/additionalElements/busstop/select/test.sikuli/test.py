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

# go to select mode
netedit.selectMode()

# netedit selection reference
selectType = netedit.getSelectReference(match)

# select all busStops with lanes that contains "E1_0" and remove it
netedit.selectItems(selectType, "Additional", "busStop", "lane", "E0_0")
netedit.deleteSelectedItems()

# select all busStops with id "busStop_IDToRemove1" and remove it
netedit.selectItems(selectType, "Additional", "busStop", "id", "=busStop_IDToRemove1")
netedit.deleteSelectedItems()

# select all busStops with id "busStop_IDToRemove2" and remove it
netedit.selectItems(selectType, "Additional", "busStop", "id", "=busStop_IDToRemove2")
netedit.deleteSelectedItems()

# select all busStops with startPos greather than 15
netedit.selectItems(selectType, "Additional", "busStop", "startPos", ">14")

# change to remove mode
netedit.modificationModeRemove(match)

# select all busStops with startPos greather than 25
netedit.selectItems(selectType, "Additional", "busStop", "startPos", ">24")

# change to add mode and remove selected busStops
netedit.modificationModeAdd(match)
netedit.deleteSelectedItems()

# select all busStops with endPos minor that 30 and delete it 
netedit.selectItems(selectType, "Additional", "busStop", "endPos", "<36")
netedit.deleteSelectedItems()

# select all busStops with lines that contains "lineToRemove" and remove it
netedit.selectItems(selectType, "Additional", "busStop", "lines", "lineToRemove")
netedit.deleteSelectedItems()

# select all busStops with lines that contains "linetoRemove" and remove it
netedit.selectItems(selectType, "Additional", "busStop", "lines", "linetoRemove")
netedit.deleteSelectedItems()

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)