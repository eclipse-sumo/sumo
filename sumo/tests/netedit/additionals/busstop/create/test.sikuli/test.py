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

# Check that default parameters are correct
try:
    find(netEditResources + "additionals/busStop-defaultParameters.png")
except:
    netEditProcess.kill()
    sys.exit("Killed netedit process. 'busStop-defaultParameters.png' not found")

# Add three extra lines
click(match.getTarget().offset(-310, 200))
click(match.getTarget().offset(-310, 225))
click(match.getTarget().offset(-310, 250))

# Set name of lines
doubleClick(match.getTarget().offset(-280, 175))
type("Line1" + Key.ENTER)
doubleClick(match.getTarget().offset(-280, 200))
type("Line2" + Key.ENTER)
doubleClick(match.getTarget().offset(-280, 225))
type("Line3" + Key.ENTER)
doubleClick(match.getTarget().offset(-280, 250))
type("Line4" + Key.ENTER)

# Remove last line
click(match.getTarget().offset(-280, 275))

# create busstop in mode "reference left"
click(match.getTarget().offset(200, 400))

# Go to reference mode
click(match.getTarget().offset(-300, 340))

# Change to reference right
click(match.getTarget().offset(-300, 390))

# create busstop in mode "reference right"
click(match.getTarget().offset(100, 400))

# Go to reference mode
click(match.getTarget().offset(-300, 340))

# Change to reference center
click(match.getTarget().offset(-300, 410))

# create busstop in mode "reference center
click(match.getTarget().offset(0, 400))

# change default lenght to 20
doubleClick(match.getTarget().offset(-270, 370))
type("100" + Key.ENTER)

# Go to reference mode
click(match.getTarget().offset(-300, 340))

# Change to reference right
click(match.getTarget().offset(-300, 390))

# try to create busstop with a different lenght WITHOUT forcing position.
# BusStop musn't be created
click(match.getTarget().offset(-100, 400))

# Go to reference mode
click(match.getTarget().offset(-300, 340))

# Change to reference left
click(match.getTarget().offset(-300, 365))

# try to create busstop with a different lenght WITHOUT forcing position.
# BusStop musn't be created
click(match.getTarget().offset(300, 400))

# Enable force position
click(match.getTarget().offset(-350, 400))

# Go to reference mode
click(match.getTarget().offset(-300, 340))

# Change to reference right
click(match.getTarget().offset(-300, 390))

# Create busstop with a different lenght forcing position.
click(match.getTarget().offset(-100, 400))

# Go to reference mode
click(match.getTarget().offset(-300, 340))

# Change to reference left
click(match.getTarget().offset(-300, 365))

# Create busstop with a different lenght forcing position.
click(match.getTarget().offset(300, 400))

# Check Undo (5 busStops)
for x in range(0, 5):
    try:
        click(match.getTarget().offset(-325, 5))
        click(netEditResources + "undoredo/edit-undo.png")
    except:
        netEditProcess.kill()
        sys.exit("Killed netedit process. 'edit-undo.png' not found")

# Check Redo (5 busStops)
for x in range(0, 5):
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
