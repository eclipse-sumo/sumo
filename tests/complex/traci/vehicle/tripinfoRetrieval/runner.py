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

# @file    runner.py
# @author  Mirko Barthauer (Technische Universitaet Braunschweig)
# @date    2018-09-27

from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net4.net.xml",
             "-r", "input_routes.rou.xml",
             "--device.tripinfo.probability", "1",
             "--no-step-log",
             ] + sys.argv[1:])


def printStats(t):
    print(t)
    vehs = traci.vehicle.getIDList()
    if vehs:
        ego = vehs[0]
        print(" veh=%s" % ego)
        for key in ['waitingTime', 'waitingCount', 'stopTime']:
            print("    ", key, traci.vehicle.getParameter(ego, "device.tripinfo." + key))
    globalKeys = [
            "count",
            "routeLength",
            "speed",
            "duration",
            "waitingTime",
            "timeLoss",
            "departDelay",
            "departDelayWaiting",
            "totalTravelTime",
            "totalDepartDelay",
            "vehicleTripStatistics.count",
            "vehicleTripStatistics.routeLength",
            "vehicleTripStatistics.speed",
            "vehicleTripStatistics.duration",
            "vehicleTripStatistics.waitingTime",
            "vehicleTripStatistics.timeLoss",
            "vehicleTripStatistics.departDelay",
            "vehicleTripStatistics.departDelayWaiting",
            "vehicleTripStatistics.totalTravelTime",
            "vehicleTripStatistics.totalDepartDelay",

            "bikeTripStatistics.count",
            "bikeTripStatistics.routeLength",
            "bikeTripStatistics.speed",
            "bikeTripStatistics.duration",
            "bikeTripStatistics.waitingTime",
            "bikeTripStatistics.timeLoss",
            "bikeTripStatistics.totalTravelTime",

            "pedestrianStatistics.count",
            "pedestrianStatistics.number",
            "pedestrianStatistics.routeLength",
            "pedestrianStatistics.duration",
            "pedestrianStatistics.timeLoss",

            "rideStatistics.count",
            "rideStatistics.number",
            "rideStatistics.waitingTime",
            "rideStatistics.routeLength",
            "rideStatistics.duration",
            "rideStatistics.bus",
            "rideStatistics.train",
            "rideStatistics.taxi",
            "rideStatistics.bike",
            "rideStatistics.aborted",

            "transportStatistics.count",
            "transportStatistics.number",
            "transportStatistics.waitingTime",
            "transportStatistics.routeLength",
            "transportStatistics.duration",
            "transportStatistics.bus",
            "transportStatistics.train",
            "transportStatistics.taxi",
            "transportStatistics.bike",
            "transportStatistics.aborted",
            ]

    for key in globalKeys:
        print(" ", key, traci.simulation.getParameter("", "device.tripinfo." + key))


while traci.simulation.getMinExpectedNumber() > 0:
    t = traci.simulation.getTime()
    if t % 10 == 0:
        printStats(t)
    traci.simulationStep()

printStats("final")


traci.close()
