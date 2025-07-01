#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
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
This script runs the gaming scenarios which have a custom tls file
with the "actuated" and the "delay_based" control and inserts the resulting scores
as a reference into the refscores.pkl file.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import subprocess
import sys
import glob
import pickle
import re

from runner import computeScore, parseEndTime, REFSCOREFILE

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa


def computeHighScore(scen, high, alg=""):
    score = computeScore(scen)
    i = 0
    for i, s in enumerate(high[scen]):
        if s[2] < score[0]:
            break
    high[scen].insert(i, ("SUMO " + alg, "", score[0]))


def main():
    base = os.path.dirname(os.path.abspath(__file__))
    high = {}
    for config in sorted(glob.glob(os.path.join(base, "*.sumocfg"))):
        scen = os.path.basename(config[:-8])
        if "demo" in scen or "bs3" in scen:
            continue
        tls = None
        add = []
        for a in sumolib.xml.parse_fast(config, "additional-files", "value"):
            for f in a.value.split(","):
                add.append(os.path.join(base, f))
                if ".tll" in f or ".tls" in f:
                    tls = add[-1]
        if tls:
            with open(tls) as tls_in:
                lines = tls_in.readlines()
        else:
            lines = []
            for n in sumolib.xml.parse_fast(config, "net-file", "value"):
                net_file = os.path.join(base, n.value)
                net = sumolib.net.readNet(net_file, withPrograms=True)
            for t in net.getTrafficLights():
                xml = t.toXML()
                if 'duration="1000' in xml:
                    if not lines:
                        lines.append("<a>")
                    lines += xml.splitlines()
            if lines:
                lines.append("</a>")
                tls = net_file
        if lines:
            high[scen] = []
            for alg, minDur in (("actuated", 3), ("delay_based", 1)):
                print("running scenario '%s' with algorithm '%s'" % (scen, alg))
                with open(tls + "." + alg, "w") as tls_out:
                    for line in lines:
                        line = line.replace('type="static"', 'type="%s"' % alg)
                        if "phase" in line:
                            line = re.sub('duration="1000\d+', 'duration="10" minDur="%s" maxDur="10000' % minDur, line)
                        tls_out.write(line)
                subprocess.call([sumolib.checkBinary('sumo'), "-c", config,
                                 "-a", ",".join(add).replace(tls, tls_out.name),
                                 '-l', os.path.join(base, scen, "log"), '--duration-log.statistics',
                                 '--statistic-output', os.path.join(base, scen, 'stats.xml'),
                                 '-v', 'false', '--no-warnings', '--no-step-log',
                                 '--tripinfo-output.write-unfinished'])
                computeHighScore(scen, high, alg)
        if scen == "rail":
            high[scen] = []
            print("running scenario 'rail'")
            subprocess.call([sumolib.checkBinary('sumo'), "-c", config.replace("rail", "rail_demo"),
                             '-l', os.path.join(base, scen, "log"), '--duration-log.statistics',
                             '--statistic-output', os.path.join(base, scen, 'stats.xml'),
                             '-v', 'false', '--no-warnings', '--no-step-log',
                             '--tripinfo-output.write-unfinished', "-e", str(parseEndTime(config))])
            computeHighScore(scen, high)
    print(high)
    with open(os.path.join(base, REFSCOREFILE), 'wb') as pkl:
        pickle.dump(high, pkl)


if __name__ == "__main__":
    main()
