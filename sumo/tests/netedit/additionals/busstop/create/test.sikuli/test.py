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

# Focus netedit window
click(match)
	
# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

# Add three extra lines
doubleClick(find(neteditResources + "attributes/attributeLines0.png").offset(75, 0))
paste("Line1")

click(find(neteditResources + "additionalMode/add.png"))
doubleClick(find(neteditResources + "attributes/attributeLines1.png").offset(75, 0))
paste("Line2")

click(find(neteditResources + "additionalMode/add.png"))
doubleClick(find(neteditResources + "attributes/attributeLines2.png").offset(75, 0))
paste("Line3")

click(find(neteditResources + "additionalMode/add.png"))
doubleClick(find(neteditResources + "attributes/attributeLines3.png").offset(75, 0))
paste("Line4")

# remove last line
click(find(neteditResources + "additionalMode/remove.png"))

# create busstop in mode "reference left"
click(match.getTarget().offset(450, 300))

# change reference to right
click(find(neteditResources + "additionalMode/referenceLeft.png"))
type(Key.DOWN)

# create busstop in mode "reference right"
click(match.getTarget().offset(300, 300))

# change reference to center
click(find(neteditResources + "additionalMode/referenceRight.png"))
type(Key.DOWN)

# create busstop in mode "reference center"
click(match.getTarget().offset(350, 300))

# return to mode "reference left"
click(find(neteditResources + "additionalMode/referenceCenter.png"))
type(Key.UP)
type(Key.UP)

# Change length
doubleClick(find(neteditResources + "attributes/attributeLength.png").offset(75, 0))
paste("30")

# try to create a busstop (Warning)
click(match.getTarget().offset(500, 300))

# change reference to right
click(find(neteditResources + "additionalMode/referenceLeft.png"))
type(Key.DOWN)

# try busstop in mode "reference right" (Warning)
click(match.getTarget().offset(250, 300))

# return to mode "reference left"
click(find(neteditResources + "additionalMode/referenceCenter.png"))
type(Key.UP)

# enable force position
click(find(neteditResources + "additionalMode/attributeForcePosition.png"))

# create a busstop forcing position
click(match.getTarget().offset(500, 300))

# change reference to right
click(find(neteditResources + "additionalMode/referenceLeft.png"))
type(Key.DOWN)

# create a busstop forcing position
click(match.getTarget().offset(250, 300))

# save additionals
# XXX add a keyboard hotkey
click(match.getTarget().offset(-200, -80))
click(match.getTarget().offset(-200, 180))

# quit
type("q", Key.CTRL)

# confirm unsafed network
type("y", Key.ALT)
type("z", Key.ALT) # work-around misinterpreted keyboard mapping
