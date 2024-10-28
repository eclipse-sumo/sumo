# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    validate.py
# @author  Michael Behrisch
# @date    2012-01-21

from __future__ import absolute_import
from __future__ import print_function

import sys
import math
import subprocess

dDay = 1
obsTimes = {}
verbose = False


def readTimes(obsfile):
    times = []
    with open(obsfile) as ifile:
        for line in ifile:
            ll = line.split(':')
            if ll:
                times.append(
                    3600 * int(ll[0]) + 60 * int(ll[1]) + int(float(ll[2])))
    return times


def parseObsTimes():
    for i in range(0, 9):
        obsTimes[i] = []
    for i in range(1, 8):
        if dDay == 1 and i == 5:
            continue
        if dDay == 2 and i == 6:
            continue
        obsTimes[i] = readTimes('data/obstimes_%s_%s.txt' % (dDay, i))

    # convert obsTimes[][] into travel-times:
    for i in range(1, 8):
        ni = len(obsTimes[i])
        if ni == len(obsTimes[i + 1]) and ni > 100:
            for j in range(ni):
                obsTimes[i][j] = obsTimes[i + 1][j] - obsTimes[i][j]


def validate(sumoBinary):
    subprocess.call(
        [sumoBinary, "-c", "data/spd-road.sumocfg"], stdout=sys.stdout, stderr=sys.stderr)
    sys.stdout.flush()
    sys.stderr.flush()

    # analyzing the results...
    # read the empirical times
    simTimes = {}
    for i in range(0, 9):
        simTimes[i] = []

    # read the simulated times
    obs2Nr = {'obs1': 1, 'obs2': 2, 'obs3': 3,
              'obs4': 4, 'obs5': 5, 'obs6': 6, 'obs7': 7}

    with open('data/detector.xml') as ifile:
        for line in ifile:
            if line.find('<interval') != -1:
                ll = line.split('"')
                iObs = obs2Nr[ll[5]]
                if int(ll[7]) > 0:
                    simTimes[iObs].append(float(ll[1]))

    # convert simTimes[][] into travel-times:
    for i in range(1, 8):
        ni = len(simTimes[i])
        if ni == len(simTimes[i + 1]) and ni > 100:
            for j in range(ni):
                simTimes[i][j] = simTimes[i + 1][j] - simTimes[i][j]

    # compute final statistics
    err = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    errAll = 0.0
    cntAll = 0
    if verbose:
        f = open('data/sumo-obs-error.txt', 'w')
    for i in range(1, 7):
        if len(obsTimes[i]) <= 100 or len(obsTimes[i + 1]) <= 100:
            continue
        if len(obsTimes[i]) == len(simTimes[i]):
            tmp = 0.0
            for o, s in zip(obsTimes[i], simTimes[i]):
                d = o - s
                tmp += d * d
            err[i] = math.sqrt(tmp / len(obsTimes[i]))
            if verbose:
                print("%s %s" % (i, err[i]), file=f)
            errAll += err[i]
            cntAll += 1
    if verbose:
        f.close()

    # finally, write the individual travel times into a csv-file
    # this is not really needed when validate is called from calibrate as an intermediate
    # step, but it makes analyzing of the result more simple.
    # first the header
    if verbose:
        c = open('data/compare-tt.csv', 'w')
        c.write('# indx;')
        for i in range(1, 7):
            if len(obsTimes[i]) > 100 and len(obsTimes[i + 1]) > 100:
                c.write('obs%s;sim%s;' % (i, i))
        c.write('\n')

        # then the data, of course on the ones which are useable
        for line in range(len(simTimes[1])):
            c.write(repr(line) + ';')
            for i in range(1, 7):
                if len(obsTimes[i]) > 100 and len(obsTimes[i + 1]) > 100:
                    ttObs = int(obsTimes[i][line])
                    ttSim = int(simTimes[i][line])
                    c.write(repr(ttObs) + ';' + repr(ttSim) + ';')
            c.write('\n')
        c.close()
    return errAll / cntAll
