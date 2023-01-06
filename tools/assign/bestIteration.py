#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    bestIteration.py
# @author  Jakob Erdmann
# @date    2021-05-10

"""
Run duaIterate and collect statistics on the iterations
"""
import os
import sys
import subprocess
import glob
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
from sumolib.statistics import Statistics  # noqa
from sumolib.xml import parse  # noqa
from shutil import copyfile  # noqa

a = sys.argv[1:]

key = ""
key += "L" if "--logit" in a else "G"
key += "_meso" if ("--mesosim" in a or "-m" in a) else ""
key += "_mesojc" if ("--meso-junctioncontrol" in a or "-j" in a) else ""
key += "_dso" if "--marginal-cost" in a else "_due"
if "--marginal-cost.exp" in a:
    key += "_e%s" % a[a.index("--marginal-cost.exp") + 1]
if "--logittheta" in a:
    key += "_t%s" % a[a.index("--logittheta") + 1]
if "--convergence-steps" in a:
    key += "_c%s" % a[a.index("--convergence-steps") + 1]
for endOpt in ['-e', '--end', 'sumo--end']:
    if endOpt in a:
        key += "_end%s" % a[a.index(endOpt) + 1]
if '_' in a[0]:
    key += a[0][a[0].find('_'):-3]
if "--extra-key" in a:
    # argument only for bestIteration.py
    index = a.index("--extra-key")
    key += "_%s" % a[index + 1]
    a.pop(index + 1)
    a.pop(index)


print("key", key)

if not os.path.isdir(key):
    os.makedirs(key)

os.chdir(key)
subprocess.call(a)
TTT = Statistics("TTT")
ATT = Statistics("ATT")
values = []
for name in glob.glob('*/stats.xml'):
    # print(name)
    for stats in parse(name, 'statistics'):
        vStats = stats.vehicleTripStatistics[0]
        vehs = stats.vehicles[0]
        ttt = float(vStats.totalTravelTime) + float(vStats.totalDepartDelay)
        att = ttt / int(vehs.loaded)
        TTT.add(ttt, name)
        ATT.add(att, name)
        values.append((att, ttt, vStats.totalTravelTime, vStats.totalDepartDelay,
                       vStats.duration, vStats.departDelay, name))

with open('results.txt', 'w') as f:
    f.write('#' '\t'.join(['att', 'cttt', 'ttt', 'tdd', 'tt', 'dd', 'file']) + '\n')
    for dat in values:
        f.write('\t'.join(map(str, dat)) + '\n')

    f.write('\n# %s\n# %s\n' % (TTT, ATT))

copyfile('results.txt', os.path.join('..', 'results_%s.txt' % key))
