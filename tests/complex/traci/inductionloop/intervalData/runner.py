#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from xml.etree import ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
detID = traci.inductionloop.getIDList()[0]

for step in range(120):
    print("%s iO=%.2f iS=%.2f iN=%s iI=%s lO=%.2f lS=%.2f lN=%s lI=%s" % (
        step,
        traci.inductionloop.getIntervalOccupancy(detID),
        traci.inductionloop.getIntervalMeanSpeed(detID),
        traci.inductionloop.getIntervalVehicleNumber(detID),
        traci.inductionloop.getIntervalVehicleIDs(detID),
        traci.inductionloop.getLastIntervalOccupancy(detID),
        traci.inductionloop.getLastIntervalMeanSpeed(detID),
        traci.inductionloop.getLastIntervalVehicleNumber(detID),
        traci.inductionloop.getLastIntervalVehicleIDs(detID)))
    traci.simulationStep()

traci.close()


if "--mesosim" not in sys.argv[1:]:
    # The slow vehicle spans an interval boundary. Compare each snapshot to the detector output
    # for the corresponding interval, including a detector without file output.
    traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "-r", "input_routes2.rou.xml",
                 "-a", "input_additional2.add.xml", "--no-step-log", "--no-duration-log", "--precision", "6"]
                + sys.argv[1:])
    samples = {}
    try:
        for t in range(1, 26):
            traci.simulationStep()
            if t >= 10:
                samples[t] = [
                    (traci.inductionloop.getLastIntervalVehicleNumber(detID),
                     traci.inductionloop.getLastIntervalVehicleIDs(detID),
                     traci.inductionloop.getLastIntervalOccupancy(detID),
                     traci.inductionloop.getLastIntervalMeanSpeed(detID))
                    for detID in ("written", "unwritten")
                ]
    finally:
        traci.close()

    intervals = ET.parse("completed-interval-e1.xml").findall("interval")
    assert [int(interval.get("nVehContrib")) for interval in intervals[:2]] == [0, 1]
    for interval in intervals[:2]:
        end = int(float(interval.get("end")))
        count = int(interval.get("nVehContrib"))
        expectedIDs = ("slowCar",) if count else ()
        for t in range(end, min(end + 10, 26)):
            for sample in samples[t]:
                assert sample[0] == count, (t, sample, interval.attrib)
                assert sample[1] == expectedIDs, (t, sample, interval.attrib)
                assert abs(sample[2] - float(interval.get("occupancy"))) < 1e-6, (t, sample, interval.attrib)
                assert abs(sample[3] - float(interval.get("speed"))) < 1e-6, (t, sample, interval.attrib)
    print("Completed interval values match detector output")
