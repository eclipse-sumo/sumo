#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@date    2013-10-19
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
import shutil
import sumodump
import sumoInductLoop
import sumotripinfos
import sumodetectors


def call(command, log):
    print("-" * 79, file=log)
    print(command, file=log)
    log.flush()
    print(command)
    retCode = subprocess.call(command, stdout=sys.stdout, stderr=sys.stderr)
    if retCode != 0:
        print("Execution of %s failed. Look into %s for details." % (
            command, log.name), file=sys.stderr)
        sys.exit(retCode)


def iterateGreenTimes(emissionClass, outputFile, T, MIN_GREEN, log):
    fd = open("input_routes.rou.xml", "w")
    print('<routes>', file=fd)
    print(
        '    <!-- vType id="t1" accel="0.8" decel="4.5" sigma="0.5" length="7.5" maxSpeed="36"/ -->	', file=fd)
    print('    <route id="r1" multi_ref="x" edges="e2 e w w2"/>', file=fd)
    for i in range(0, 1000):
        print('    <vehicle id="v.%s" depart="%s" departspeed="13.89" departpos="0" route="r1" type="t1"/>' % (
            i, i * 5), file=fd)
    print('</routes>', file=fd)
    fd.close()

    fdo = open(outputFile, "w")
    for i in range(MIN_GREEN, T - MIN_GREEN):
        print(">>> Building plan (at t=%s)" % (i))
        fd = open("input_additional.add.xml", "w")
        print('<additional>', file=fd)
        print('   <vType id="t1" accel="0.8" decel="4.5" sigma="0.5" length="7.5" maxSpeed="36" aemissionClass="%s"/>' %
              emissionClass, file=fd)
        print(
            '   <tlLogic id="c" type="static" programID="my" offset="0.00">', file=fd)
        print('      <phase duration="%s" state="GGrr"/>' % (i), file=fd)
        print('      <phase duration="%s" state="rrGG"/>' % (T - i), file=fd)
        print('   </tlLogic>', file=fd)
        print(
            '   <laneData id="traffic" freq="100000" file="aggregated_traffic.xml"/>', file=fd)
        print(
            '   <laneData id="noise" freq="100000" type="harmonoise" file="aggregated_noise.xml"/>', file=fd)
        print(
            '   <laneData id="emissions" freq="100000" type="hbefa" file="aggregated_emissions.xml"/>', file=fd)
        print(
            '   <e2Detector id="e2" lane="e_0" pos=".1" length="9999.8" freq="100000" file="e2.xml" friendlyPos="x"/>', file=fd)
        print(
            '   <e2Detector id="e2_tls" tl="c" to="w_0" lane="e_0" pos=".1" length="9999.8" freq="100000" file="e2_tls.xml" friendlyPos="x"/>', file=fd)
        print('</additional>', file=fd)
        fd.close()

        print(">>> Simulating (at t=%s)" % (i))
        call(sumoBinary + " -c sumo.sumo.cfg -v", log)

#    shutil.copy("aggregated_traffic.xml", "backup/" + str(i) + "_aggregated_traffic.xml")
#    shutil.copy("aggregated_noise.xml", "backup/" + str(i) + "_aggregated_noise.xml")
#    shutil.copy("aggregated_emissions.xml", "backup/" + str(i) + "_aggregated_emissions.xml")
#    shutil.copy("tripinfos.xml", "backup/" + str(i) + "_tripinfos.xml")
#    shutil.copy("e2.xml", "backup/" + str(i) + "_e2.xml")
#    shutil.copy("e2_tls.xml", "backup/" + str(i) + "_e2_tls.xml")
#    shutil.copy("netstate.xml", "backup/" + str(i) + "_netstate.xml")

        dumpEmissions = sumodump.readDump("aggregated_emissions.xml", [
                                          "CO_perVeh", "CO2_perVeh", "HC_perVeh", "PMx_perVeh", "NOx_perVeh", "fuel_perVeh"])
        dumpNoise = sumodump.readDump("aggregated_noise.xml", ["noise"])
        dumpTraffic = sumodump.readDump(
            "aggregated_traffic.xml", ["waitingTime", "speed"])
        tripinfos = sumotripinfos.readTripinfos("tripinfos.xml", ["waitSteps"])
        e2 = sumodetectors.readAreal("e2.xml", [
                                     "maxJamLengthInVehicles", "jamLengthInVehiclesSum", "meanHaltingDuration", "startedHalts"])

        vals = tripinfos.get("waitSteps")
        meanV = 0
        for v in vals:
            meanV += vals[v]
        meanV = meanV / len(vals)

        wtv = dumpTraffic.get("waitingTime")[-1]["e_0"]
        nv = dumpNoise.get("noise")[-1]["e_0"]
        cov = dumpEmissions.get("CO_perVeh")[-1]["e_0"] / 1000.
        co2v = dumpEmissions.get("CO2_perVeh")[-1]["e_0"] / 1000.
        hcv = dumpEmissions.get("HC_perVeh")[-1]["e_0"] / 1000.
        pmv = dumpEmissions.get("PMx_perVeh")[-1]["e_0"] / 1000.
        nov = dumpEmissions.get("NOx_perVeh")[-1]["e_0"] / 1000.
        fv = dumpEmissions.get("fuel_perVeh")[-1]["e_0"] / 1000.
        msv = dumpTraffic.get("speed")[-1]["e_0"]

        mjlv = e2.get("maxJamLengthInVehicles")[-1]["e2"]
        jlsv = e2.get("jamLengthInVehiclesSum")[-1]["e2"]
        mhdv = e2.get("meanHaltingDuration")[-1]["e2"]
        shv = e2.get("startedHalts")[-1]["e2"]

        print("%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s" % (
            T - i, wtv, nv, cov, co2v, hcv, pmv, nov, fv, meanV, msv, 10000. / msv - 10000. / 13.82, mjlv, jlsv, mhdv, shv), file=fdo)
    fdo.close()


MIN_GREEN = 10
T = 180
log = open("logfile.txt", "w")

# os.environ.get("NETCONVERT_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', 'bin', 'netconvert'))
netconvertBinary = ".\\netconvert.exe"
call(netconvertBinary + " -c netconvert.netc.cfg -v", log)
sumoBinary = ".\\sumo.exe"

emissionClasses = []
fd = open("all.txt")
for line in fd.readlines():
    line = line.strip()
    if len(line) < 1:
        continue
    emissionClasses.append(line)

for e in emissionClasses:
    of = e + "_results.csv"
    iterateGreenTimes(e, of, T, MIN_GREEN, log)
