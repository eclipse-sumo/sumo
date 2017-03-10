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

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("dummyElement")
# set elementClass
type(Key.TAB)
# abort current selection
netedit.abortSelection()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("Net Element")
# jump to element
for x in range(0, 2):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementType
paste("dummyType")
# type tab to set elementType
type(Key.TAB)
# abort current selection
netedit.abortSelection()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.focusOnFrame()
for x in range(0, 5):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("Net Element")
# jump to element
for x in range(0, 2):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementType
paste("junction")
# jump to attribute
for x in range(0, 2):
    type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new attribute
paste("dummyAttribute")
# type enter to set attribute
type(Key.TAB)
# abort current selection
netedit.abortSelection()

# try to select node gneJ0 and delete it (Error in type of element)
netedit.selectItems("Net Element", "junction", "id", "gneJ0")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
