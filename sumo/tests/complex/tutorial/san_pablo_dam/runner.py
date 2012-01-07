#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2007-10-25
@version $Id$

This script is a test runner for the San Pablo Dam scenario.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""


import os,subprocess,sys
import shutil
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), "..", "..", "..")), "tools"))
from sumolib import checkBinary


netconvertBinary = checkBinary('netconvert')
# build/check network
retcode = subprocess.call([netconvertBinary, "-n", "data/spd-road.nod.xml", "-e", "data/spd-road.edg.xml", "-o", "data/spd-road.net.xml","-v"], stdout=sys.stdout, stderr=sys.stderr)
try: shutil.copy("data/spd-road.net.xml", "net.net.xml")
except: print "Missing 'spd-road.net.xml'"
print ">> Netbuilding closed with status %s" % retcode
sys.stdout.flush()
# build/check vss
os.chdir("data")
exec(compile(open('make-vss-file.py').read(), 'make-vss-file.py', 'exec'))
try: shutil.copy("spd-road.vss.xml", "../result1")
except: print "Missing 'spd-road.vss.xml'"
sys.stdout.flush()
# perform calibration
exec(compile(open('calibrate.py').read(), 'calibrate.py', 'exec'))
try: shutil.copy("all-the-results.txt", "../result2")
except: print "Missing 'all-the-results.txt'"
sys.stdout.flush()


