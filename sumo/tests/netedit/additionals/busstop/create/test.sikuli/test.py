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
currentEnvironmentFile = open(SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
# Get path to netEdit app
neteditApp = currentEnvironmentFile.readline().replace("\n", "")
# Get SandBox folder
textTestSandBox = currentEnvironmentFile.readline().replace("\n", "")
# Get resources depending of the current Operating system
currentOS = currentEnvironmentFile.readline().replace("\n", "")
neteditResources = SUMOFolder + "/tests/netedit/imageResources/" + currentOS + "/"
neteditReference = SUMOFolder + "/tests/netedit/imageResources/reference.png"
currentEnvironmentFile.close()
#****#

# Open netedit
neteditProcess = subprocess.Popen([neteditApp,
                                   '--gui-testing',
                                   '--window-size', '700,500',
                                   '--new',
                                   '--additionals-output', textTestSandBox + "/additionals.xml"],
                                   env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# Wait to netedit and focus
try:
    match = wait(neteditReference, 20)
except:
    neteditProcess.kill()
    sys.exit("Killed netedit process. 'reference.png' not found")

# Focus netedid window
click(match.getTarget().offset(0, -105))
	
# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

# Add three extra lines
click(match.getTarget().offset(-145, 130))
click(match.getTarget().offset(-145, 155))
click(match.getTarget().offset(-145, 180))

# Set name of lines
doubleClick(match.getTarget().offset(-135, 100))
type("Line1" + Key.ENTER)
doubleClick(match.getTarget().offset(-135, 125))
type("Line2" + Key.ENTER)
doubleClick(match.getTarget().offset(-135, 150))
type("Line3" + Key.ENTER)
doubleClick(match.getTarget().offset(-135, 175))
type("Line4" + Key.ENTER)

# Remove last line
click(match.getTarget().offset(-110, 195))

# create busstop in mode "reference left"
click(match.getTarget().offset(400, 300))

# save additionals
click(match.getTarget().offset(-200, -80))
click(match.getTarget().offset(-200, 180))

# quit
type("q", Key.CTRL)

type("y", Key.ALT)
