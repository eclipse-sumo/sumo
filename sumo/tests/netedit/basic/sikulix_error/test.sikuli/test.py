#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
netEditResources = os.environ['SUMO_HOME'] + "/tests/netedit/imageResources/"

# abort function


def abort(process, reason):
    process.kill()
    sys.exit("Killed netedit process. '" + reason + "' not found")
#****#

# Import libraries
import os
import sys
import subprocess

# Open netedit
netEditProcess = subprocess.Popen([os.environ['NETEDIT_BINARY'],
                                   '--window-size', '800,600'],
                                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

# Settings.MinSimilarity = 0.1
try:
    wait(netEditResources + "neteditIcon.png", 60)
except:
    abort(netEditProcess, "neteditIcon.png")

# Try to click over a non existent image (edit-redo.png)
# This provoke the following line in siculixLog: [error] SystemExit (
# Killed netedit process. 'edit-redo.png or toolbar-file.png' not found )
try:
    click(netEditResources + "toolbar/toolbar-file.png")
    click(netEditResources + "toolbar/toolbar-edit/edit-redo.png")
except:
    abort(netEditProcess, "edit-redo.png or toolbar-file.png")

# quit
type("q", Key.CTRL)
try:
    find(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
    type("y", Key.ALT)
except:
    abort(netEditProcess, "dialog-confirmClosingNetwork")
