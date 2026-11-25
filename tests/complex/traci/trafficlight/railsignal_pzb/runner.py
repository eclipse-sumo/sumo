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

import os
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


SIGNAL = "10713172896"


def set_visibility(source, target, distance):
    net = ET.parse(source)
    connection = net.find(f"./connection[@tl='{SIGNAL}'][@from='248740648#4.1425'][@to='441932172#1']")
    connection.set("visibility", str(distance))
    net.write(target)


def approach(net="input_net.net.xml", routes="input_routes.rou.xml", pzb=False,
             configure=False, release=True, release_at_stop=False):
    args = [sumolib.checkBinary("sumo"), "-n", net, "-r", routes, "--no-step-log",
            "--xml-validation", "never", "--time-to-teleport", "-1"]
    if configure:
        args += ["-a", "input_additional.add.xml"]
    traci.start(args)
    result = []
    crossed = False
    released = False
    try:
        if pzb:
            traci.trafficlight.setParameter(SIGNAL, "pzb", "true")
        if pzb or configure:
            assert traci.trafficlight.getParameter(SIGNAL, "pzb") == "true"
        for _ in range(160):
            traci.simulationStep()
            if "follower" not in traci.vehicle.getIDList():
                continue
            tls = traci.vehicle.getNextTLS("follower")
            if not tls or tls[0][0] != SIGNAL:
                crossed = True
                break
            _, _, distance, aspect = tls[0]
            speed = traci.vehicle.getSpeed("follower")
            result.append((distance, speed, aspect))
            if release and not released and "blocker" in traci.vehicle.getIDList():
                ready = speed < 0.01 and distance < 100 if release_at_stop else distance < 625
                if ready and traci.vehicle.isStopped("blocker"):
                    traci.vehicle.resume("blocker")
                    released = True
    finally:
        traci.close()
    return result, crossed


set_visibility(sys.argv[1], "input_net.net.xml", 80)
default, default_crossed = approach()
pzb, pzb_crossed = approach(pzb=True)
configured, configured_crossed = approach(configure=True)
held_red, red_crossed = approach(release=False)
assert default_crossed and pzb_crossed and configured_crossed and not red_crossed
assert pzb == configured
first_green = next(i for i, row in enumerate(pzb) if row[2] == "G")
assert pzb[:first_green] == default[:first_green]
assert pzb[0][2] == "r" and pzb[first_green][0] > 200
print("Red limits both approaches before the signal clears.")
first_visible = next(i for i, row in enumerate(pzb) if row[0] <= 81)
assert first_green < first_visible
assert [r[:2] for r in pzb[:first_visible + 1]] == [r[:2] for r in held_red[:first_visible + 1]]
assert pzb[first_visible + 1][1] > pzb[first_visible][1]
assert default[first_green][1] > pzb[first_green][1]
print("PZB delays acceleration after a clear signal until it is visible.")

routes = ET.parse("input_routes.rou.xml")
routes.getroot().remove(routes.find("./vehicle[@id='blocker']"))
routes.write("steady.rou.xml")
steady, steady_crossed = approach(routes="steady.rou.xml", pzb=True)
assert steady_crossed and all(row[2] == "G" for row in steady)
assert [r[:2] for r in steady[:first_visible + 1]] == [r[:2] for r in held_red[:first_visible + 1]]
print("A steady green signal is also subject to PZB visibility.")

routes = ET.parse("input_routes.rou.xml")
routes.find("vType").set("jmStoplineGap", "25")
routes.write("offset.rou.xml")
set_visibility(sys.argv[1], "narrow.net.xml", 0)
stopped, stopped_crossed = approach(net="narrow.net.xml", routes="offset.rou.xml", pzb=True, release_at_stop=True)
assert stopped_crossed
assert any(distance >= 25 and speed == 0 and aspect == "r" for distance, speed, aspect in stopped)
assert any(distance < 25 and speed > 0 and aspect == "G" for distance, speed, aspect in stopped)
print("A train stopped at its configured gap can start when green.")


def revoke_visible_green():
    downstream_signal = "10713172887"
    traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "-r", "steady.rou.xml",
                 "--no-step-log", "--xml-validation", "never", "--time-to-teleport", "-1"])
    slowed = False
    revoked_at = None
    stopped = False
    try:
        traci.trafficlight.setParameter(SIGNAL, "pzb", "true")
        for step in range(300):
            traci.simulationStep()
            if "follower" not in traci.vehicle.getIDList():
                continue
            tls = traci.vehicle.getNextTLS("follower")
            if not tls or tls[0][0] != SIGNAL:
                assert stopped
                return
            _, _, distance, aspect = tls[0]
            speed = traci.vehicle.getSpeed("follower")
            if not slowed and distance < 300:
                # Keep a brakeable approach when imposing the new red aspect.
                traci.vehicle.setMaxSpeed("follower", 5)
                slowed = True
            if revoked_at is None and distance <= 30 and aspect == "G":
                traci.trafficlight.addConstraint(SIGNAL, "follower", downstream_signal, "absent", limit=1)
                revoked_at = step
            elif revoked_at is not None and not stopped:
                assert aspect == "r"
                if speed == 0:
                    assert distance > 0
                    traci.trafficlight.removeConstraints(SIGNAL, "follower", downstream_signal, "absent")
                    stopped = True
        raise AssertionError("Train did not cross after the signal cleared again")
    finally:
        traci.close()


revoke_visible_green()
print("A red aspect takes effect immediately after visible green.")
