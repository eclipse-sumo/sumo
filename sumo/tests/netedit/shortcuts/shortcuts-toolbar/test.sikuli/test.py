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

# locate netEditIcon
try:
    wait(netEditResources + "neteditIcon.png", 60)
except:
    abort(netEditProcess, "neteditIcon.png")

# this test requieres a similarity of 0.9
Settings.MinSimilarity = 0.9

# click in the center of netEdit (needed for activate shortcuts)
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400, 300))

# open file toolbar
try:
    type("f", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-file-selected.png")
except:
    abort(netEditProcess, "toolbar-file-selected.png")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-file/file-allDisabled.png")
except:
    abort(netEditProcess, "file-allDisabled.png")

# close toolbar file
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-file.png")
except:
    abort(netEditProcess, "toolbar-file.png")

# open edit toolbar
try:
    type("e", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-edit-selected.png")
except:
    abort(netEditProcess, "toolbar-edit")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-edit/edit-allDisabled.png")
except:
    abort(netEditProcess, "edit-allDisabled.png")

# close edit file
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-edit.png")
except:
    abort(netEditProcess, "toolbar-edit.png")

# open Processing toolbar
try:
    type("p", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-processing-selected.png")
except:
    abort(netEditProcess, "toolbar-processing-selected.png")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-processing/processing-allDisabled.png")
except:
    abort(netEditProcess, "processing-allDisabled.png")

# close processing
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-processing.png")
except:
    abort(netEditProcess, "toolbar-processing.png")

# open Locate toolbar
try:
    type("l", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-locate-selected.png")
except:
    abort(netEditProcess, "toolbar-locate-selected.png")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-locate/locate-allDisabled.png")
except:
    abort(netEditProcess, "locate-allDisabled.png")

# close locate
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-locate.png")
except:
    abort(netEditProcess, "locate-processing.png")

# open window toolbar
try:
    type("w", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-window-selected.png")
except:
    abort(netEditProcess, "toolbar-window-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-window/window-allEnabled.png")
except:
    abort(netEditProcess, "window-allEnabled.png")

# close window
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-window.png")
except:
    abort(netEditProcess, "window-processing.png")

# open Help toolbar
try:
    type("h", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-help-selected.png")
except:
    abort(netEditProcess, "toolbar-help-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-help/help-allEnabled.png")
except:
    abort(netEditProcess, "help-allEnabled.png")

# close help
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-help.png")
except:
    abort(netEditProcess, "help-processing.png")

# Create new net to inspect the menus
type("a", Key.CTRL)

# Change to create mode
type("e")

# Create two edges
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(200, 200))
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(600, 200))
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(200, 300))
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(600, 300))

# undo create scond edge
type("z", Key.CTRL)

# open file toolbar
try:
    type("f", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-file-selected.png")
except:
    abort(netEditProcess, "toolbar-file-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-file/file-allEnabled.png")
except:
    abort(netEditProcess, "file-allEnabled.png")

# close toolbar file
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-file.png")
except:
    abort(netEditProcess, "toolbar-file.png")

# open edit toolbar
try:
    type("e", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-edit-selected.png")
except:
    abort(netEditProcess, "toolbar-edit")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-edit/edit-allEnabled.png")
except:
    abort(netEditProcess, "edit-allEnabled.png")

# close edit file
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-edit.png")
except:
    abort(netEditProcess, "toolbar-edit.png")

# open Processing toolbar
try:
    type("p", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-processing-selected.png")
except:
    abort(netEditProcess, "toolbar-processing-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-processing/processing-allEnabled.png")
except:
    abort(netEditProcess, "processing-allEnabled.png")

# close processing
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-processing.png")
except:
    abort(netEditProcess, "toolbar-processing.png")

# open Locate toolbar
try:
    type("l", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-locate-selected.png")
except:
    abort(netEditProcess, "toolbar-locate-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-locate/locate-allEnabled.png")
except:
    abort(netEditProcess, "locate-allEnabled.png")

# close locate
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-locate.png")
except:
    abort(netEditProcess, "locate-processing.png")

# open window toolbar
try:
    type("w", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-window-selected.png")
except:
    abort(netEditProcess, "toolbar-window-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-window/window-allEnabled.png")
except:
    abort(netEditProcess, "window-allEnabled.png")

# close window
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-window.png")
except:
    abort(netEditProcess, "window-processing.png")

# open Help toolbar
try:
    type("h", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-help-selected.png")
except:
    abort(netEditProcess, "toolbar-help-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-help/help-allEnabled.png")
except:
    abort(netEditProcess, "help-allEnabled.png")

# close help
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-help.png")
except:
    abort(netEditProcess, "help-processing.png")

# close network
type("w", Key.CTRL)
try:
    find(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
    type("y", Key.ALT)
except:
    abort(netEditProcess, "dialog-confirmClosingNetwork")

# open file toolbar
try:
    type("f", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-file-selected.png")
except:
    abort(netEditProcess, "toolbar-file-selected.png")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-file/file-allDisabled.png")
except:
    abort(netEditProcess, "file-allDisabled.png")

# close toolbar file
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-file.png")
except:
    abort(netEditProcess, "toolbar-file.png")

# open edit toolbar
try:
    type("e", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-edit-selected.png")
except:
    abort(netEditProcess, "toolbar-edit")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-edit/edit-allDisabled.png")
except:
    abort(netEditProcess, "edit-allDisabled.png")

# close edit file
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-edit.png")
except:
    abort(netEditProcess, "toolbar-edit.png")

# open Processing toolbar
try:
    type("p", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-processing-selected.png")
except:
    abort(netEditProcess, "toolbar-processing-selected.png")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-processing/processing-allDisabled.png")
except:
    abort(netEditProcess, "processing-allDisabled.png")

# close processing
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-processing.png")
except:
    abort(netEditProcess, "toolbar-processing.png")

# open Locate toolbar
try:
    type("l", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-locate-selected.png")
except:
    abort(netEditProcess, "toolbar-locate-selected.png")

# check if all menus are disabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-locate/locate-allDisabled.png")
except:
    abort(netEditProcess, "locate-allDisabled.png")

# close locate
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-locate.png")
except:
    abort(netEditProcess, "locate-processing.png")

# open window toolbar
try:
    type("w", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-window-selected.png")
except:
    abort(netEditProcess, "toolbar-window-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-window/window-allEnabled.png")
except:
    abort(netEditProcess, "window-allEnabled.png")

# close window
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-window.png")
except:
    abort(netEditProcess, "window-processing.png")

# open Help toolbar
try:
    type("h", Key.ALT)
    wait(netEditResources + "toolbar/toolbar-help-selected.png")
except:
    abort(netEditProcess, "toolbar-help-selected.png")

# check if all menus are enabled as espected
try:
    wait(netEditResources + "toolbar/toolbar-help/help-allEnabled.png")
except:
    abort(netEditProcess, "help-allEnabled.png")

# close help
try:
    type(Key.ESC)
    wait(netEditResources + "toolbar/toolbar-help.png")
except:
    abort(netEditProcess, "help-processing.png")

# close netedit
type("q", Key.CTRL)
