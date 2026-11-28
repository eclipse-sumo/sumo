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
# @author  Mark Colley
# @date    2026-11-18

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net4.net.xml",
             "-r", "input_routes.rou.xml",
             "-a", "input_additional.add.xml",
             "--no-step-log",
             "--stop-output", "stopinfos.xml", "--stop-output.write-unfinished",
             "--device.taxi.dispatch-algorithm", "traci",
             ] + sys.argv[1:])

TAXI = "taxi_0"


def stops():
    return [(s.stoppingPlaceID or s.lane, s.actType) for s in traci.vehicle.getStops(TAXI)]


traci.simulationStep()
print("keepChargingStops", traci.vehicle.getParameter(TAXI, "device.taxi.keepChargingStops"))
r0 = traci.person.getTaxiReservations(0)[0].id
traci.vehicle.dispatchTaxi(TAXI, [r0])
# plan a charging stop after the drop-off
traci.vehicle.insertStop(TAXI, len(traci.vehicle.getStops(TAXI)), "cs0", duration=20, flags=tc.STOP_CHARGING_STATION)
print("time=%s after dispatch stops=%s" % (traci.simulation.getTime(), stops()))

redispatched = False
idleRedirect = False
lastOutput = None
while traci.simulation.getTime() < 500:
    traci.simulationStep()
    t = traci.simulation.getTime()
    state = traci.vehicle.getParameter(TAXI, "device.taxi.state")
    if not redispatched:
        new = [r for r in traci.person.getTaxiReservations(0) if r.id != r0]
        if new and traci.vehicle.getPersonNumber(TAXI) > 0:
            # re-dispatch from scratch: the customer on board is mentioned once, the new customer twice
            traci.vehicle.dispatchTaxi(TAXI, [r0, new[0].id, new[0].id])
            redispatched = True
            print("time=%s after re-dispatch stops=%s" % (t, stops()))
    elif not idleRedirect and t >= 320 and state == "0" and traci.vehicle.isStoppedTriggered(TAXI):
        # send the idle taxi (waiting at its last charging stop) to two other charging stations
        traci.vehicle.resume(TAXI)
        traci.vehicle.insertStop(TAXI, 0, "cs1", duration=10, flags=tc.STOP_CHARGING_STATION)
        traci.vehicle.insertStop(TAXI, 1, "cs2", duration=10, flags=tc.STOP_CHARGING_STATION)
        idleRedirect = True
        print("time=%s idle redirect stops=%s" % (t, stops()))
    output = (state, traci.vehicle.isStoppedTriggered(TAXI), stops())
    if output != lastOutput:
        print("time=%s state=%s edge=%s triggered=%s stops=%s" % (
            t, output[0], traci.vehicle.getRoadID(TAXI), output[1], output[2]))
        lastOutput = output

traci.close()
