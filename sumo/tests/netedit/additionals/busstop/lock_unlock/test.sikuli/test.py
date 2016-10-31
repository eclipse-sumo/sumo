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

# Create two nodes
click(match.getTarget().offset(-250, 400))
click(match.getTarget().offset(250, 400))

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

# Go to reference mode
click(match.getTarget().offset(-300, 300))

# Change to reference center
click(match.getTarget().offset(-300, 360))

# create busstop in mode "reference center"
click(match.getTarget().offset(100, 400))

# Change to movement mode
type("m")

# Change mouse move delay
Settings.MoveMouseDelay = 1

# Move unlocked
dragDrop(match.getTarget().offset(100, 405),
         match.getTarget().offset(-100, 405))
dragDrop(match.getTarget().offset(-100, 405),
         match.getTarget().offset(100, 405))

# Change back mouse move delay
Settings.MoveMouseDelay = 0.1

# Change to inspect mode
type("i")

# inspect busStop
click(match.getTarget().offset(100, 405))

# Go to reference mode
click(match.getTarget().offset(-330, 330))

# Change to movement mode
type("m")

# Change back mouse move delay
Settings.MoveMouseDelay = 1

# Try to move locked busstop
dragDrop(match.getTarget().offset(100, 405),
         match.getTarget().offset(-100, 405))

# Change back mouse move delay
Settings.MoveMouseDelay = 0.1

# Check Undo of block movement
try:
    click(match.getTarget().offset(-325, 5))
    click(netEditResources + "undoredo/edit-undo.png")
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'edit-undo.png' not found")

# Check Redo of block movement
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
