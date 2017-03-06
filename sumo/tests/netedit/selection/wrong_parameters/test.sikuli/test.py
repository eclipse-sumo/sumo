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

# try to select node gneJ0 and delete it (Error in type of element)
click(selectType)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("dummyElement")
# type two times enter to set elementClass
for x in range(0, 2):
	type(Key.ENTER)

# try to select node gneJ0 and delete it (Error in type of element)
click(selectType)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("Net Element")
# type two times enter to set elementClass
for x in range(0, 2):
	type(Key.ENTER)
# jump to element
for x in range(0, 2):
	type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementType
paste("dummyType")
# type enter to set elementType
type(Key.ENTER)

# try to select node gneJ0 and delete it (Error in type of element)
click(selectType)
# select all
type("a", Key.CTRL)
# paste the new elementClass
paste("Net Element")
# type two times enter to set elementClass
for x in range(0, 2):
	type(Key.ENTER)
# jump to element
for x in range(0, 2):
	type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new elementType
paste("junction")
# type enter to set elementType
type(Key.ENTER)
# jump to attribute
for x in range(0, 2):
	type(Key.TAB)
# select all
type("a", Key.CTRL)
# paste the new attribute
paste("dummyAttribute")
# type enter to set attribute
type(Key.ENTER)

# try to select node gneJ0 and delete it (Error in type of element)
netedit.selectItems(selectType, "Net Element", "junction", "id", "gneJ0")
netedit.deleteSelectedItems()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)
