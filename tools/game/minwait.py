#!/usr/bin/env python
# -*- coding: utf8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    minwait.py
# @author  Michael Behrisch
# @date    2023-03-29

"""
This script runs the gaming GUI for the LNdW traffic light game.
It checks for possible scenarios in the current working directory
and lets the user start them as a game. Furthermore it
saves highscores to local disc and to the central highscore server.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import subprocess
import sys
import glob
import pickle

from runner import computeScoreFromWaitingTime, MAXSCOREFILE

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa


base = os.path.dirname(os.path.abspath(__file__))
high = {}
for config in sorted(glob.glob(os.path.join(base, "*.sumocfg"))):
    scen_path = config[:-8]
    scen = os.path.basename(scen_path)
    if "demo" in scen:
        continue
    tls = None
    add = []
    for a in sumolib.xml.parse_fast(config, "additional-files", "value"):
        for f in a.value.split(","):
            add.append(os.path.join(base, f))
            if ".tll" in f or ".tls" in f:
                tls = add[-1]
    if tls:
        high[scen] = []
        for alg in ("actuated", "delay_based"):
            with open(tls) as tls_in, open(tls + "." + alg, "w") as tls_out:
                for line in tls_in:
                    line = line.replace('type="static"', 'type="%s"' % alg)
                    if "phase" in line:
                        line = line.replace('duration="10000"', 'duration="10" minDur="1" maxDur="10000"')
                    tls_out.write(line)
            subprocess.call([sumolib.checkBinary("sumo"), "-c", config, "-a", ",".join(add).replace(tls, tls_out.name),
                             '--duration-log.statistics', '--statistic-output', scen_path + '.stats.xml',
                             '--tripinfo-output.write-unfinished'])
            score = computeScoreFromWaitingTime(scen_path)
            i = 0
            for i, s in enumerate(high[scen]):
                if s[2] < score[0]:
                    break
            high[scen].insert(i, ("SUMO " + alg, "", score[0]))
print(high)
with open(os.path.join(base, MAXSCOREFILE), 'wb') as pkl:
    pickle.dump(high, pkl)
