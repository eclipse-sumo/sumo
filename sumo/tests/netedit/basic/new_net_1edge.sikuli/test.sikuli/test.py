Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1

import os, sys, subprocess
subprocess.Popen([os.environ['NETEDIT_BINARY'], 
                  '--window-size', '800,600',
                  '--new', 
                  '-o', 'net.net.xml'], 
                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
wait("netediticon.png")


# focus
click(Pattern("netediticon.png").targetOffset(30,0))

# Change to create mode
type("e")


# Create two nodes
click(Pattern("netediticon.png").targetOffset(200,400))
click(Pattern("netediticon.png").targetOffset(600,400))

# save network and quit
type("s", Key.CTRL)
type("q", Key.CTRL)




