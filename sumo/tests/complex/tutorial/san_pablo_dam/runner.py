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
import numpy as np
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), "..", "..", "..")), "tools"))
sys.path.append(os.path.join(os.path.dirname(__file__), 'data'))
from sumolib import checkBinary

def buildVSS(obs7file, obs8file, vss):
    t7Times = []
    for line in open(obs7file):
        ll = line.split(':')
        if ll:
            t7Times.append(3600*int(ll[0]) + 60*int(ll[1]) + int(float(ll[2])))
    t8Times = []
    for line in open(obs8file):
        ll = line.split(':')
        if ll:
            t8Times.append(3600*int(ll[0]) + 60*int(ll[1]) + int(float(ll[2])))
    print 'data read: ', len(t7Times), len(t8Times)

    fp = open(vss, 'w')
    lObs8 = 337.5
    print >> fp, '<vss>'
    for i, t7 in enumerate(t7Times):
        v = lObs8/(t8Times[i] - t7)
        if i != len(t7Times)-1 and t7 != t7Times[i+1]:
            print >> fp, '    <step time ="%s" speed="%s"/>' % (t7, v)
    print >> fp, '</vss>'
    fp.close()


netconvertBinary = checkBinary('netconvert')
# build/check network
retcode = subprocess.call([netconvertBinary, "-n", "data/spd-road.nod.xml", "-e", "data/spd-road.edg.xml", "-o", "data/spd-road.net.xml","-v"], stdout=sys.stdout, stderr=sys.stderr)
try: shutil.copy("data/spd-road.net.xml", "net.net.xml")
except: print "Missing 'spd-road.net.xml'"
print ">> Netbuilding closed with status %s" % retcode
sys.stdout.flush()
# build/check vss
buildVSS('data/obstimes_1_7.txt', 'data/obstimes_1_8.txt', 'data/spd-road.vss.xml')
shutil.copy("data/spd-road.vss.xml", "vss.xml")
sys.stdout.flush()
# perform calibration
os.chdir("data")
import calibrate
try: shutil.copy("all-the-results.txt", "../results.csv")
except: print "Missing 'all-the-results.txt'"
sys.stdout.flush()
