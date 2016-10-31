# Import libraries
import os
import sys
import subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
# SUMO Folder
SUMOFolder = os.environ.get('SUMO_HOME', '.')
# Current environment
currentEnvironmentFile = open(
    SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
# Get path to netEdit app
neteditApp = currentEnvironmentFile.readline().replace("\n", "")
# Get SandBox folder
textTestSandBox = currentEnvironmentFile.readline().replace("\n", "")
# Get resources depending of the current Operating system
currentOS = currentEnvironmentFile.readline().replace("\n", "")
netEditResources = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/"
currentEnvironmentFile.close()
#****#

# Open netedit
netEditProcess = subprocess.Popen([neteditApp,
                                   '--window-size', '800,600',
                                   '--new',
                                   '--additionals-output', textTestSandBox + "/additionals.xml"],
                                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# Wait 10 seconds to netedit main windows
try:
    match = wait(netEditResources + "neteditToolbar.png", 10)
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'neteditToolbar.png' not found")

# focus netEdit window
click(match.getTarget().offset(0, -20))

# Change to create mode
type("e")

# Create first two nodes
click(match.getTarget().offset(-250, 200))
click(match.getTarget().offset(250, 200))

# Create second two nodes
click(match.getTarget().offset(-250, 400))
click(match.getTarget().offset(250, 400))

# Change to inspect
type("i")

# get menu of lane and duplicate it
rightClick(match.getTarget().offset(0, 200))
click(match.getTarget().offset(50, 500))

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

# Go to reference mode
click(match.getTarget().offset(-300, 300))

# Change to reference center
click(match.getTarget().offset(-300, 360))

# create first busstop in mode "reference center"
click(match.getTarget().offset(100, 400))

# create second busstop in mode "reference center"
click(match.getTarget().offset(0, 210))

# Change to inspect
type("i")

# inspect first busstop
click(match.getTarget().offset(100, 405))

# check that default values are correct
try:
    find(netEditResources + "additionals/busStop-defaultAttributes.png")
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'neteditToolbar.png' not found")

# Change parameter 1 with a non valid value (Duplicated ID)
doubleClick(match.getTarget().offset(-250, 120))
type("busStop_gneE0_1_1" + Key.ENTER)

# Change parameter 1 with a valid value
doubleClick(match.getTarget().offset(-250, 120))
type("correct ID" + Key.ENTER)

# Change parameter 2 with a non valid value (dummy lane)
doubleClick(match.getTarget().offset(-250, 160))
type("dummyLane" + Key.ENTER)

# Change parameter 2 with a valid value (different edge)
doubleClick(match.getTarget().offset(-250, 160))
type("gneE0_0" + Key.ENTER)

# Change parameter 2 with a valid value (same edge, different lane)
doubleClick(match.getTarget().offset(-250, 160))
type("gneE0_1" + Key.ENTER)

# Change parameter 3 with a non valid value (negative)
doubleClick(match.getTarget().offset(-250, 200))
type("-5" + Key.ENTER)

# Change parameter 3 with a non valid value (> endPos)
doubleClick(match.getTarget().offset(-250, 200))
type("200" + Key.ENTER)

# Change parameter 3 with a valid value
doubleClick(match.getTarget().offset(-250, 200))
type("20" + Key.ENTER)

# Change parameter 4 with a non valid value (out of range, and not accepted)
doubleClick(match.getTarget().offset(-250, 240))
type("3000" + Key.ENTER)

# Wait to dialog
try:
    wait(netEditResources + "endPositionOutOfRangeDialog.png")
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'endPositionOutOfRangeDialog.png' not found")
type("n", Key.ALT)

# Change parameter 4 with a valid value (out of range, but adapted to the
# end of lane)
doubleClick(match.getTarget().offset(-250, 240))
type("3000" + Key.ENTER)

# Wait to dialog
try:
    wait(netEditResources + "endPositionOutOfRangeDialog.png")
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'endPositionOutOfRangeDialog.png' not found")
type("y", Key.ALT)

# Change parameter 4 with a non valid value (<startPos)
doubleClick(match.getTarget().offset(-250, 240))
type("10" + Key.ENTER)

# Change parameter 4 with a valid value
doubleClick(match.getTarget().offset(-250, 240))
type("30" + Key.ENTER)

# Change parameter 5 with a non valid value (throw warning)
doubleClick(match.getTarget().offset(-250, 280))
type("line1, line2" + Key.ENTER)

# Change parameter 5 with a valid value
doubleClick(match.getTarget().offset(-250, 280))
type("line1 line2" + Key.ENTER)

# go to a empty area
click(match.getTarget().offset(0, 450))

# Check UndoRedo (8 attribute changes, 1 creation)
for x in range(0, 9):
    try:
        click(match.getTarget().offset(-325, 5))
        click(netEditResources + "undoredo/edit-undo.png")
    except:
        netEditProcess.kill()
        sys.exit("Killed netedit process. 'edit-undo.png' not found")

for x in range(0, 9):
    try:
        click(match.getTarget().offset(-325, 5))
        click(netEditResources + "undoredo/edit-redo.png")
    except:
        netEditProcess.kill()
        sys.exit("Killed netedit process. 'edit-redo.png' not found")

# save additionals
click(match.getTarget().offset(-375, 5))
click(match.getTarget().offset(-350, 265))

# quit
type("q", Key.CTRL)
try:
    find(netEditResources + "confirmClosingNetworkDialog.png")
    type("y", Key.ALT)
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'confirmClosingNetworkDialog.png' not found")
