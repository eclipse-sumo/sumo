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

# obtain match for additionalsComboBox
additionalsComboBox = match.getTarget().offset(-75, 50)
	
# Focus netedit window
click(match)
	
# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100, 300))
click(match.getTarget().offset(500, 300))

# Change to create additional
type("a")

# go to additionalsComboBox
click(additionalsComboBox)

# by default, additional is busstop, then isn't needed to select "busstop"

# Add three extra lines
for x in range(0, 3):
	type(Key.TAB)
	
for x in range(0, 3):
	type(Key.SPACE)
	
#add extra lines
click(additionalsComboBox)

for x in range(0, 2):
	type(Key.TAB)
	
paste("Line1")
type(Key.TAB)
paste("Line2")
type(Key.TAB)
paste("Line3")
type(Key.TAB)
paste("Line4")

# remove last line (line 4)
click(additionalsComboBox)

for x in range(0, 7):
	type(Key.TAB)
	
type(Key.SPACE)

# create busstop in mode "reference left"
click(match.getTarget().offset(450, 300))

# change reference to right
click(additionalsComboBox)
for x in range(0, 8):
	type(Key.TAB)
	
type(Key.DOWN)

# create busstop in mode "reference right"
click(match.getTarget().offset(300, 300))

# change reference to center
click(additionalsComboBox)
for x in range(0, 8):
	type(Key.TAB)
	
type(Key.DOWN)

# create busstop in mode "reference center"
click(match.getTarget().offset(350, 300))

# return to mode "reference left"
click(additionalsComboBox)

for x in range(0, 8):
	type(Key.TAB)
	
for x in range(0, 2):
	type(Key.UP)

# Change length
click(additionalsComboBox)

for x in range(0, 10):
	type(Key.TAB)
	
paste("30")

# try to create a busstop (Warning)
click(match.getTarget().offset(500, 300))

# change reference to right
click(additionalsComboBox)

for x in range(0, 8):
	type(Key.TAB)
	
type(Key.DOWN)

# try busstop in mode "reference right" (Warning)
click(match.getTarget().offset(250, 300))

# return to mode "reference left"
click(additionalsComboBox)

for x in range(0, 8):
	type(Key.TAB)
	
type(Key.UP)

# enable force position
click(additionalsComboBox)

for x in range(0, 11):
	type(Key.TAB)
	
type(Key.SPACE)

# create a busstop forcing position
click(match.getTarget().offset(500, 300))

# change reference to right
click(additionalsComboBox)

for x in range(0, 8):
	type(Key.TAB)
	
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