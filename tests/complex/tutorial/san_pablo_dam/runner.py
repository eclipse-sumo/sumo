#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2007-10-25

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import shutil
from scipy.optimize import fmin_cobyla

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
from sumolib import checkBinary  # noqa
import validate  # noqa


def buildVSS(obs7file, obs8file, vss):
    t7Times = validate.readTimes(obs7file)
    t8Times = validate.readTimes(obs8file)
    print('data read: ', len(t7Times), len(t8Times))

    fp = open(vss, 'w')
    lObs8 = 337.5
    print('<vss>', file=fp)
    for i, t7 in enumerate(t7Times):
        v = lObs8 / (t8Times[i] - t7)
        if i != len(t7Times) - 1 and t7 != t7Times[i + 1]:
            print('    <step time ="%s" speed="%s"/>' % (t7, v), file=fp)
    print('</vss>', file=fp)
    fp.close()


def genDemand(inputFile, outputFile):
    t1Times = validate.readTimes(inputFile)
    fRou = open(outputFile, 'w')
    fRou.write('<routes>\n')
    fRou.write('    <route id="route01" edges="1to7 7to8"/>\n')
    for vehID, t in enumerate(t1Times):
        print('    <vehicle depart="%s" arrivalPos="-1" id="%s" route="route01" type="pass" departSpeed="max" />' % (
            t, vehID), file=fRou)
    print('</routes>', file=fRou)
    fRou.close()

# definition of gof() function to be given to fmin_cobyla() or fmin()


def gof(p):
    para = [('vMax', p[0]),
            ('aMax', p[1]),
            ('bMax', p[2]),
            ('lCar', p[3]),
            ('sigA', max(0, min(1, p[4]))),
            ('tTau', p[5])]
    print('# simulation with:', *["%s:%.3f" % i for i in para])
    fType = open('data/input_types.add.xml', 'w')
    fType.write(('<routes>\n    <vType accel="%(aMax)s" decel="%(bMax)s" id="pass"' +
                 ' length="%(lCar)s" minGap="2.5" maxSpeed="%(vMax)s"' +
                 ' sigma="%(sigA)s" tau="%(tTau)s" />\n</routes>') % dict(para))
    fType.close()
    result = validate.validate(checkBinary('sumo'))
    print('#### yields rmse: %.4f' % result)
    print("%s %s" % (" ".join(["%.3f" % pe for pe in p]), result), file=fpLog)
    fpLog.flush()
    return result

# defining all the constraints


def conVmax(params):  # vMax < 25
    return 25.0 - params[0]


def conTtau(params):  # tTau > 1.1
    return params[5] - 1.1


def conSigA(params):  # sigA > 0.1
    return params[4] - 0.1


def conSigA2(params):  # sigA < 1.0
    return 1.0 - params[4]


def conAmax(params):  # aMax > 0.1
    return params[1] - 0.1


netconvertBinary = checkBinary('netconvert')
# build/check network
retcode = subprocess.call([netconvertBinary, "-n", "data/spd-road.nod.xml", "-e",
                           "data/spd-road.edg.xml", "-o", "data/spd-road.net.xml", "-v"],
                          stdout=sys.stdout, stderr=sys.stderr)
try:
    shutil.copy("data/spd-road.net.xml", "net.net.xml")
except IOError:
    print("Missing 'spd-road.net.xml'")
print(">> Netbuilding closed with status %s" % retcode)
sys.stdout.flush()
# build/check vss
buildVSS('data/obstimes_1_7.txt',
         'data/obstimes_1_8.txt', 'data/spd-road.vss.xml')
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
fmin_cobyla(
    gof, params, [conVmax, conAmax, conTtau, conSigA, conSigA2], rhoend=1.0e-4)
fpLog.close()
