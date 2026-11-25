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

# @file    runner.py
# @author  Josh
# @date    2026-11-26

from contextlib import contextmanager
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


FIRST = "taxi_0"
SECOND = "taxi_1"
NEW = 1
OPEN = 3
ASSIGNED = 4
ONBOARD = 8


@contextmanager
def simulation(routes="input_routes.rou.xml", additionals=None):
    args = [sumolib.checkBinary("sumo"), "-n", "input_net4.net.xml", "-r", routes,
            "--no-step-log", "--device.taxi.dispatch-algorithm", "traci"]
    if additionals is not None:
        args += ["-a", additionals]
    traci.start(args + sys.argv[1:])
    try:
        traci.simulationStep()
        assert set(traci.vehicle.getTaxiFleet(0)) == {FIRST, SECOND}
        reservations = {r.persons[0]: r.id for r in traci.person.getTaxiReservations()}
        yield reservations["a"], reservations["b"]
    finally:
        traci.close()


def reservations(state):
    return {r.id for r in traci.person.getTaxiReservations(state)}


def stops(vehicle):
    return [(stop.lane, stop.startPos, stop.endPos, stop.stopFlags) for stop in traci.vehicle.getStops(vehicle)]


def test_mixed_plan():
    with simulation() as (a, b):
        traci.vehicle.dispatchTaxi(FIRST, [a, b, a, b])
        for _ in range(150):
            if traci.vehicle.getPersonIDList(FIRST) == ("a",):
                break
            traci.simulationStep()
        assert reservations(ONBOARD) == {a}
        assert reservations(ASSIGNED) == {b}
        assert not reservations(OPEN)

        previous = stops(FIRST)
        try:
            traci.vehicle.dispatchTaxi(FIRST, [], replace=True)
        except traci.TraCIException:
            pass
        else:
            raise AssertionError("Clearing a taxi with an onboard passenger must fail")
        assert stops(FIRST) == previous
        assert reservations(ONBOARD) == {a}
        assert reservations(ASSIGNED) == {b}
        assert not reservations(OPEN)
        print("Rejected clearing an onboard passenger's plan without changing reservations or stops.")

        traci.vehicle.dispatchTaxi(FIRST, [a], replace=True)
        assert reservations(ONBOARD) == {a}
        assert reservations(NEW) == {b}
        assert not reservations(NEW)
        assert reservations(OPEN) == {b}
        assert all(stop[0] not in {"B1C1_0", "C1D1_0"} for stop in stops(FIRST))
        assert not reservations(ASSIGNED)
        traci.vehicle.dispatchTaxi(SECOND, [b])
        assert reservations(ASSIGNED) == {b}
        assert not reservations(OPEN)

        arrived = set()
        b_boarded = False
        for _ in range(300):
            traci.simulationStep()
            assert "b" not in traci.vehicle.getPersonIDList(FIRST)
            assert "a" not in traci.vehicle.getPersonIDList(SECOND)
            b_boarded |= "b" in traci.vehicle.getPersonIDList(SECOND)
            arrived.update(traci.simulation.getArrivedPersonIDList())
            if arrived == {"a", "b"}:
                break
        assert b_boarded and arrived == {"a", "b"}
        assert not traci.person.getTaxiReservations()
        print("Kept the onboard dropoff and served the same reopened reservation with the other taxi.")


def test_clear_waiting():
    with simulation() as (a, b):
        traci.vehicle.dispatchTaxi(FIRST, [a, b, a, b])
        assert reservations(ASSIGNED) == {a, b}
        assert not reservations(ONBOARD)
        traci.vehicle.dispatchTaxi(FIRST, [], replace=True)
        assert reservations(OPEN) == {a, b}
        assert not reservations(ASSIGNED)
        assert FIRST in traci.vehicle.getTaxiFleet(0)
        traci.vehicle.dispatchTaxi(SECOND, [a, b, a, b])
        assert reservations(ASSIGNED) == {a, b}
        print("Cleared an all-waiting plan and reassigned both original reservation IDs.")


def test_disjoint_plan_and_ownership():
    with simulation() as (a, b):
        traci.vehicle.dispatchTaxi(FIRST, [a])
        traci.vehicle.dispatchTaxi(SECOND, [b])
        assert reservations(ASSIGNED) == {a, b}
        assert not reservations(ONBOARD)
        before = stops(FIRST), stops(SECOND)
        try:
            traci.vehicle.dispatchTaxi(FIRST, [b], replace=True)
        except traci.TraCIException:
            pass
        else:
            raise AssertionError("A replacement must not take a reservation assigned to another taxi")
        assert (stops(FIRST), stops(SECOND)) == before
        assert reservations(ASSIGNED) == {a, b}
        assert not reservations(OPEN)

        traci.vehicle.dispatchTaxi(SECOND, [], replace=True)
        traci.vehicle.dispatchTaxi(FIRST, [b], replace=True)
        assert reservations(ASSIGNED) == {b}
        assert reservations(OPEN) == {a}
        assert SECOND in traci.vehicle.getTaxiFleet(0)
        print("Rejected taking another taxi's reservation and accepted a disjoint replacement once it was open.")


def test_legacy_append():
    with simulation() as (a, b):
        traci.vehicle.dispatchTaxi(FIRST, [a])
        traci.vehicle.dispatchTaxi(FIRST, [b])
        assert reservations(ASSIGNED) == {a, b}
        assert not reservations(OPEN)
        try:
            traci.vehicle.dispatchTaxi(FIRST, [])
        except traci.TraCIException:
            pass
        else:
            raise AssertionError("Legacy dispatch must reject an empty plan")
        assert reservations(ASSIGNED) == {a, b}
        print("Legacy dispatch still appends an unassigned reservation and rejects an empty plan.")


def test_denied_badge_preserves_plan():
    with simulation("input_routes2.rou.xml", "input_additional.add.xml") as (a, b):
        traci.vehicle.dispatchTaxi(FIRST, [a])

        def plan():
            return (traci.vehicle.getRoute(FIRST), tuple(str(stop) for stop in traci.vehicle.getStops(FIRST)),
                    {r.id: r.state for r in traci.person.getTaxiReservations()})

        before = plan()
        try:
            traci.vehicle.dispatchTaxi(FIRST, [b], replace=True)
        except traci.TraCIException as error:
            assert "badge" in str(error)
        else:
            raise AssertionError("The replacement requires a parking badge the taxi lacks")
        assert plan() == before, "Rejected replacement changed the route, stops or assignment"
        assert reservations(ASSIGNED) == {a}
        assert reservations(OPEN) == {b}
        print("Rejected a restricted parking stop without changing the previous taxi plan or reservations.")


test_mixed_plan()
test_clear_waiting()
test_disjoint_plan_and_ownership()
test_legacy_append()
test_denied_badge_preserves_plan()
