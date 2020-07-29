import os
import sys
import subprocess


SUMO_HOME = os.path.dirname(__file__)


def _makefunc(app):
    return lambda: sys.exit(subprocess.call([os.path.join(SUMO_HOME, 'bin', app)] + sys.argv[1:]))


activitygen = _makefunc("activitygen")
dfrouter = _makefunc("dfrouter")
duarouter = _makefunc("duarouter")
emissionsDrivingCycle = _makefunc("emissionsDrivingCycle")
emissionsMap = _makefunc("emissionsMap")
jtrrouter = _makefunc("jtrrouter")
marouter = _makefunc("marouter")
netconvert = _makefunc("netconvert")
netedit = _makefunc("netedit")
netgenerate = _makefunc("netgenerate")
od2trips = _makefunc("od2trips")
polyconvert = _makefunc("polyconvert")
sumo = _makefunc("sumo")
sumo_gui = _makefunc("sumo-gui")
