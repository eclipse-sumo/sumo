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

# Open netEdit
netEditProcess = subprocess.Popen([os.environ['NETEDIT_BINARY'],
                                   '--window-size', '800,600',
                                   '--new',
                                   '-o', 'net.net.xml'],
                                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
try:
    wait(netEditResources + "neteditIcon.png", 60)
except:
    abort(netEditProcess, "neteditIcon.png")

# Close netEdit
type("q", Key.CTRL)
