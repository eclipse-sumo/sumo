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

# @file    analyzeData.py
# @author  Daniel Krajzewicz
# @author  Laura Bieker
# @date    2011-09-30

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import numpy as np


def getAttr(line, which):
    beg = line.find(which)
    beg = line.find('"', beg)
    end = line.find('"', beg + 1)
    return line[beg + 1:end]

# this is from here: http://code.activestate.com/recipes/389639


class Ddict(dict):

    def __init__(self, default=None):
        self.default = default

    def __getitem__(self, key):
        if key not in self:
            self[key] = self.default()
        return dict.__getitem__(self, key)

# os.system('run-an-external-command')
# os.getcwd()
# os.chdir()


f = open(sys.argv[1], 'r')
data = f.readlines()
f.close()

dd = Ddict(lambda: Ddict(lambda: 0))
# f1 = open('raw-results.txt','w')
f1 = open('tmp.txt', 'w')

for i in range(1, len(data)):
    if data[i].find('<interval') != -1:
        ll = data[i].split('"')
        nn = int(getAttr(data[i], "nVehContrib"))  # int(ll[7])
        lane = int(getAttr(data[i], "id")[-1:])  # int(ll[5])
        tt = float(getAttr(data[i], "begin"))  # float(ll[1])
        itt = int(tt)
        if nn > 0:
            print(tt, lane, nn, ll[9], ll[11], ll[13], ll[15], file=f1)
            dd[itt][lane] = nn

f1.close()
maxLanes = 0
dt2OneHour = 6.0

for t in dd.iterkeys():
    if len(dd[t]) > maxLanes:
        maxLanes = len(dd[t])

tVec = np.zeros(len(dd), dtype=int)
QVec = np.zeros(len(dd), dtype=int)
xVec = np.zeros((len(dd), maxLanes), dtype=float)
qVec = np.zeros((len(dd), maxLanes), dtype=float)
vecIndx = 0

f = open('lane-shares.txt', 'w')
# for t,v in dd.items():
for t in sorted(dd.iterkeys()):
    # qTot = math.fsum(dd[t])
    qTot = sum(dd[t].values())
    nrm = 0.0
    if qTot:
        nrm = 1.0 / qTot
    s = repr(t) + ' ' + repr(qTot) + ' '
    tVec[vecIndx] = t
    QVec[vecIndx] = dt2OneHour * qTot
    for lane in range(maxLanes):
        share = 0.0
        if lane in dd[t]:
            share = nrm * dd[t][lane]
        s = s + repr(share) + ' '
        xVec[vecIndx, lane] = share
        qVec[vecIndx, lane] = dt2OneHour * dd[t][lane]
        # print >> f,t,qTot,lane,share
    vecIndx += 1
    print(s, file=f)
f.close()

try:
    import matplotlib.pyplot as plt
    plt.rcParams['xtick.direction'] = 'out'
    plt.rcParams['ytick.direction'] = 'out'
    # y =
    n = len(qVec)
    for lane in range(maxLanes):
        desc = 'lane: ' + repr(lane)
        plt.plot(tVec, qVec[range(n), lane], label=desc)
    # plt.plot(tVec, qVec[range(n),0], 'r-',tVec, qVec[range(n),1], 'g-',tVec, qVec[range(n),2], 'b-')
    # plt.plot(tVec, QVec, 'r-')
    plt.ylabel('lane flows')
    plt.xlabel('time [s]')
    plt.legend()
    bname = 'flows-over-time-' + repr(maxLanes)
    plt.savefig(bname + '.eps')
    plt.savefig(bname + '.pdf')
    plt.savefig(bname + '.png')
    plt.savefig(bname + '.svg')
    # try:
    # import pyemf
    # plt.savefig('shares-over-time.emf')
    # except :
    # print '# no emf support'
    # plt.show()
    plt.close()
    # next plot:
    for lane in range(maxLanes):
        desc = 'lane: ' + repr(lane)
        plt.plot(QVec, xVec[range(n), lane], 'o', markersize=10, label=desc)
    # plt.plot(tVec, qVec[range(n),0], 'r-',tVec, qVec[range(n),1], 'g-',tVec, qVec[range(n),2], 'b-')
    # plt.plot(tVec, QVec, 'r-')
    plt.ylabel('lane shares')
    plt.xlabel('total flow [veh/h]')
    plt.legend()
    bname = 'shares-vs-flow-' + repr(maxLanes)
    plt.savefig(bname + '.eps')
    plt.savefig(bname + '.pdf')
    plt.savefig(bname + '.png')
    plt.savefig(bname + '.svg')
    # plt.show()
    plt.close()
except ImportError:
    print('no matplotlib, falling back to gnuplot')
    os.system('gnuplot do-some-plots.gnu')
