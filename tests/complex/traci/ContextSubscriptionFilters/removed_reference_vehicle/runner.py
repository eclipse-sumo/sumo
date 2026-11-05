#!/usr/bin/env python
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

"""Explicit removal cancels subscriptions whose reference vehicle disappears."""

import os
import sys

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
import traci
from traci import constants as tc


def add_vehicle(veh_id, position=20):
    traci.vehicle.add(veh_id, "route", departPos=str(position))
    traci.simulationStep()
    traci.vehicle.setSpeed(veh_id, 0)


def subscribe(veh_id):
    traci.vehicle.subscribe(veh_id, [tc.VAR_SPEED])
    traci.vehicle.subscribeContext(veh_id, tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED])


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--no-step-log"])
try:
    traci.route.add("route", ["edge"])
    add_vehicle("unsubscribed")
    traci.vehicle.remove("unsubscribed")
    traci.simulationStep()
    print("Unsubscribed vehicle removal keeps the simulation running")

    add_vehicle("reference", 50)
    subscribe("reference")
    add_vehicle("unrelated", 10)
    traci.vehicle.remove("unrelated")
    traci.simulationStep()
    assert set(traci.vehicle.getContextSubscriptionResults("reference")) == {"reference"}
    print("Removing an unrelated vehicle preserves the context subscription")

    traci.vehicle.remove("reference")
    traci.simulationStep()
    assert not traci.vehicle.getSubscriptionResults("reference")
    assert not traci.vehicle.getContextSubscriptionResults("reference")
    traci.simulationStep()
    print("Removing a context reference vehicle cancels its subscriptions")

    add_vehicle("arrived", 50)
    subscribe("arrived")
    traci.vehicle.remove("arrived", tc.REMOVE_ARRIVED)
    traci.simulationStep()
    assert not traci.vehicle.getSubscriptionResults("arrived")
    assert not traci.vehicle.getContextSubscriptionResults("arrived")
    print("Removing a context reference vehicle with REMOVE_ARRIVED cancels its subscriptions")

    traci.person.add("person", "edge", 20)
    traci.person.appendWaitingStage("person", 100)
    traci.simulationStep()
    traci.person.subscribeContext("person", tc.CMD_GET_PERSON_VARIABLE, 100, [tc.VAR_SPEED])
    traci.person.remove("person")
    traci.simulationStep()
    assert not traci.person.getContextSubscriptionResults("person")
    print("Removing a person cancels its context subscription")

    add_vehicle("natural", 80)
    subscribe("natural")
    traci.vehicle.setSpeed("natural", 13.89)
    for _ in range(20):
        traci.simulationStep()
        if "natural" not in traci.vehicle.getIDList():
            break
    else:
        raise AssertionError("The vehicle did not arrive")
    assert not traci.vehicle.getContextSubscriptionResults("natural")
    print("Natural vehicle arrival cancels its context subscription")
finally:
    traci.close()
