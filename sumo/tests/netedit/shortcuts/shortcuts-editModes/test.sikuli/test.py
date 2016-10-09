#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
netEditResources = os.environ['SUMO_HOME'] + "/tests/netedit/imageResources/"
#****#

# Import libraries
import os, sys, subprocess

#Open netedit
subprocess.Popen([os.environ['NETEDIT_BINARY'], 
                  '--window-size', '800,600',
                  '--new', 
                  '-o', 'net.net.xml'], 
                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
wait(netEditResources + "neteditIcon.png")

# 1 - Check Shorcuts of edit modes

# 1.1 - create edge
type("e")
wait(netEditResources + "editModes/editMode-createEdge.png")

# 1.2 - move
type("m")
wait(netEditResources + "editModes/editMode-move.png")

# 1.3 - delete
type("d")
wait(netEditResources + "editModes/editMode-delete.png")

# 1.4 - inspect
type("i")
wait(netEditResources + "editModes/editMode-inspect.png")

# 1.5 - select
type("s")
wait(netEditResources + "editModes/editMode-select.png")

# 1.6 - connect
type("c")
wait(netEditResources + "editModes/editMode-connect.png")

# 1.7 - traffic lights
type("t")
wait(netEditResources + "editModes/editMode-trafficLight.png")

# 1.8 - additionals
type("a")
wait(netEditResources + "editModes/editMode-additionals.png")

# quit
type("q", Key.CTRL)