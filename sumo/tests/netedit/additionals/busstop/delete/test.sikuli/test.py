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

def neteditUndo(match):
	type("e", Key.ALT)
	try:
		click(neteditResources + "/undoredo/edit-undo.png")
		click(match)
	except:
		neteditProcess.kill()
		sys.exit("Killed netedit process. 'edit-undo.png' not found")
	
def neteditRedo(match):
	type("e", Key.ALT)
	try:
		click(neteditResources + "/undoredo/edit-redo.png")
		click(match)
	except:
		neteditProcess.kill()
		sys.exit("Killed netedit process. 'edit-redo.png' not found")
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

# create busstop in mode "reference left"
click(match.getTarget().offset(450, 300))

# Change to delete
type("d")

# delete created busstop
click(match.getTarget().offset(460, 315))

# Check undo redo
neteditUndo(match)
neteditRedo(match)

# save additionals
# XXX add a keyboard hotkey
click(match.getTarget().offset(-200, -80))
click(match.getTarget().offset(-200, 180))

# quit
type("q", Key.CTRL)

# confirm unsafed network
type("y", Key.ALT)
type("z", Key.ALT) # work-around misinterpreted keyboard mapping
