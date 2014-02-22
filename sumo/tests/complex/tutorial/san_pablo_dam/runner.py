#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2007-10-25
@version $Id$

This script is a test runner for the San Pablo Dam scenario.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,subprocess,sys
import shutil
from scipy.optimize import fmin_cobyla

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
from sumolib import checkBinary
import validate

def buildVSS(obs7file, obs8file, vss):
    t7Times = validate.readTimes(obs7file)
    t8Times = validate.readTimes(obs8file)
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

def genDemand(inputFile, outputFile):
    t1Times = validate.readTimes(inputFile)
    fRou = open(outputFile, 'w')
    fRou.write('<routes>\n')
    fRou.write('    <route id="route01" edges="1to7 7to8"/>\n')
    for vehID, t in enumerate(t1Times):
        print >> fRou, '    <vehicle depart="%s" arrivalPos="-1" id="%s" route="route01" type="pass" departSpeed="max" />' % (t, vehID)
    print >> fRou, '</routes>'
    fRou.close()

# definition of gof() function to be given to fmin_cobyla() or fmin() 
def gof(p):
    para = {'vMax': p[0], 'aMax': p[1], 'bMax': p[2], 'lCar': p[3], 'sigA': p[4], 'tTau': p[5]}
    print '# simulation with:',
    for k, v in para.items():
        print "%s:%.3f" % (k,v),
    print
    fType = open('data/input_types.add.xml', 'w')
    fType.write(('<routes>\n    <vType accel="%(aMax)s" decel="%(bMax)s" id="pass"' +
                ' length="%(lCar)s" minGap="2.5" maxSpeed="%(vMax)s"' + 
                ' sigma="%(sigA)s" tau="%(tTau)s" />\n</routes>') % para)
    fType.close()
    result = validate.validate(checkBinary('sumo'))
    print '#### yields rmse: %.4f' % result
    print >> fpLog, "%s %s" % (" ".join(["%.3f" % pe for pe in p]), result)
    fpLog.flush()
    return result

# defining all the constraints
def conVmax(params): # vMax < 25
    return 25.0 - params[0]

def conTtau(params): # tTau > 1.1
    return params[5] - 1.1

def conSigA(params): # sigA > 0.1
    return params[4] - 0.1

def conSigA2(params): # sigA < 1.0
    return 1.0 - params[4]

def conAmax(params): # aMax > 0.1
    return params[1] - 0.1


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
genDemand('data/obstimes_1_1.txt', 'data/spd-road.rou.xml')
validate.parseObsTimes()
# perform calibration
fpLog = open('results.csv', 'w')
params = [22.0, 2.0, 2.0, 5.0, 0.5, 1.5]
# call to (unconstrained) Nelder Mead; does not work correctly, because 
# method very often stumples over unrealistic input parameters (like tTau<1),
# which causes SUMO to behave strangely.
# fmin(gof, params)
fmin_cobyla(gof, params, [conVmax, conAmax, conTtau, conSigA, conSigA2], rhoend=1.0e-4)
fpLog.close()
