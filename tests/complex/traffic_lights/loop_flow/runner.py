#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2021 German Aerospace Center (DLR) and others.
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
# @date    2007-07-26


from __future__ import absolute_import
from __future__ import print_function
import sys
import os
import subprocess
import random
import shutil

random.seed(42)
SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import sumolib  # noqa

types = ["static", "actuated", "sotl_phase", "sotl_platoon", "sotl_request", "sotl_wave", "sotl_marching", "swarm"]
tlType = sys.argv[1]
flow1def = "0;2000;600".split(";")
flow2def = "0;2000;600".split(";")
fillSteps = 120  # 3600
measureSteps = 600  # 36000
simSteps = fillSteps + measureSteps


def buildDemand(simSteps, pWE, pEW, pNS, pSN):
    fd = open("input_routes.rou.xml", "w")
    # ---routes---
    print("""<routes>

            <vType id="type1" accel="2.0" decel="5.0" sigma="0.0" length="6.5" maxSpeed="70"/>

            <route id="WE" edges="1i 3o 5o"/>
            <route id="NS" edges="2i 4o 6o"/>
            <route id="EW" edges="3i 1o 7o"/>
            <route id="SN" edges="4i 2o 8o"/>

        """, file=fd)
    vehNr = 0
    for i in range(simSteps):
        if random.uniform(0, 1) < pWE:  # Poisson distribution
            print('    <vehicle id="%i" type="type1" route="WE" depart="%i" departSpeed="13.89" />' % (
                vehNr, i), file=fd)
            vehNr += 1
        if random.uniform(0, 1) < pNS:
            print('    <vehicle id="%i" type="type1" route="NS" depart="%i" departSpeed="13.89" />' % (
                vehNr, i), file=fd)
            vehNr += 1
        if random.uniform(0, 1) < pEW:
            print('    <vehicle id="%i" type="type1" route="EW" depart="%i" departSpeed="13.89" />' % (
                vehNr, i), file=fd)
            vehNr += 1
        if random.uniform(0, 1) < pSN:
            print('    <vehicle id="%i" type="type1" route="SN" depart="%i" departSpeed="13.89" />' % (
                vehNr, i), file=fd)
            vehNr += 1
    print("</routes>", file=fd)
    fd.close()


def patchTLSType(ifile, itype, ofile, otype):
    fdi = open(ifile)
    fdo = open(ofile, "w")
    for line in fdi:
        line = line.replace(itype, otype)
        fdo.write(line)
    fdo.close()
    fdi.close()


def main():
    try:
        os.mkdir("results")
    except OSError:
        pass
    try:
        os.mkdir("gfx")
    except OSError:
        pass

    sumo = sumolib.checkBinary('sumo')

    for f1 in range(int(flow1def[0]), int(flow1def[1]), int(flow1def[2])):
        pWE = float(f1) / 3600  # [veh/s]
        pEW = pWE
        for f2 in range(int(flow2def[0]), int(flow2def[1]), int(flow2def[2])):
            pNS = float(f2) / 3600  # [veh/s]
            pSN = pNS
            print("Computing for %s<->%s" % (f1, f2))
            buildDemand(simSteps, pWE, pEW, pNS, pSN)
            print(" for tls-type %s" % tlType)
            patchTLSType('input_additional_template.add.xml',
                         '%tls_type%', 'input_additional.add.xml', tlType)
            args = [sumo,
                    '--no-step-log',
                    # '--no-duration-log',
                    # '--verbose',
                    # '--duration-log.statistics',
                    '--default.speeddev', '0',
                    '--net-file', 'input_net.net.xml',
                    '--route-files', 'input_routes.rou.xml',
                    '--additional-files', 'input_additional.add.xml',
                    '--tripinfo-output', 'results/tripinfos_%s_%s_%s.xml' % (
                        tlType, f1, f2),
                    '--summary-output', 'results/summary_%s_%s_%s.xml' % (
                        tlType, f1, f2),
                    '--device.emissions.probability', '1',
                    '--queue-output', 'results/queue_%s_%s_%s.xml' % (
                        tlType, f1, f2),
                    '--statistic-output', 'statistics.xml',
                    ]
            subprocess.call(args)
            shutil.move(
                "results/e2_output.xml", "results/e2_output_%s_%s_%s.xml" %
                (tlType, f1, f2))
            shutil.move("results/e2_tl0_output.xml",
                        "results/e2_tl0_output_%s_%s_%s.xml" % (tlType, f1, f2))
            shutil.move("results/edgeData_3600.xml",
                        "results/edgeData_3600_%s_%s_%s.xml" % (tlType, f1, f2))
            shutil.move("results/laneData_3600.xml",
                        "results/laneData_3600_%s_%s_%s.xml" % (tlType, f1, f2))
            shutil.move("results/edgesEmissions_3600.xml",
                        "results/edgesEmissions_3600_%s_%s_%s.xml" % (tlType, f1, f2))
            shutil.move("results/lanesEmissions_3600.xml",
                        "results/lanesEmissions_3600_%s_%s_%s.xml" % (tlType, f1, f2))
            shutil.move(
                "results/TLSStates.xml", "results/TLSStates_%s_%s_%s.xml" %
                (tlType, f1, f2))
            shutil.move("results/TLSSwitchTimes.xml",
                        "results/TLSSwitchTimes_%s_%s_%s.xml" % (tlType, f1, f2))
            shutil.move("results/TLSSwitchStates.xml",
                        "tls_state.xml")


if __name__ == "__main__":
    main()
