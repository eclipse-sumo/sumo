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

# first rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# first check that invert works
netedit.selectionInvert()

# invert again and delete selected items (all must be unselected)
netedit.selectionInvert()
netedit.deleteSelectedItems()

# invert again and clear selection
netedit.selectionInvert()
netedit.selectionClear()

# use a rectangle to select central elements 
netedit.selectionRectangle(match, 250, 150, 400, 300)

# invert selection to select only extern nodes and delete it
netedit.selectionInvert()
netedit.deleteSelectedItems()

# check undo and redo
netedit.undo(match, 1)
netedit.redo(match, 2)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
