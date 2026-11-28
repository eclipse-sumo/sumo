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
# @date    2026-11-23

import os
import subprocess
import sys
from xml.etree import ElementTree

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


def check_modes(route_file):
    routes = ElementTree.parse(route_file)
    car = routes.find("person[@id='by_car']")
    taxi = routes.find("person[@id='by_taxi']")
    assert [stage.tag for stage in car] == ["ride"]
    assert car[0].get("lines") == "by_car_0"
    assert (car[0].get("from"), car[0].get("to")) == ("car_start", "car_finish")
    assert [stage.tag for stage in taxi] == ["walk", "ride"]
    assert taxi[0].get("edges") == "walk_start taxi_pickup"
    assert taxi[1].get("lines") == "taxi"
    assert (taxi[1].get("from"), taxi[1].get("to")) == ("taxi_pickup", "taxi_finish")
    return routes


network = "car_taxi.net.xml"
subprocess.check_output([sumolib.checkBinary("netconvert"), "-n", "input_nodes.nod.xml",
                         "-e", "input_edges.edg.xml", "--no-internal-links", "-o", network])
options = ["-n", network, "--persontrip.transfer.walk-taxi", "allJunctions",
           "--persontrip.transfer.taxi-walk", "allJunctions", "--no-step-log"]
subprocess.check_output([sumolib.checkBinary("duarouter"), *options, "-r", "input_routes.rou.xml",
                         "-o", "routed.rou.xml"])
routes = check_modes("routed.rou.xml")
vehicles = routes.findall("vehicle")
assert [(vehicle.get("id"), vehicle.get("depart")) for vehicle in vehicles] == [("by_car_0", "triggered")]
assert vehicles[0].find("route").get("edges") == "car_start car_finish"

subprocess.check_call([sumolib.checkBinary("sumo"), *options, "-r", "input_routes2.rou.xml,input_routes.rou.xml",
                       "--vehroute-output", "travelled.xml", "--tripinfo-output", "actual-tripinfo.xml",
                       "--end", "600", "--no-duration-log"])
check_modes("travelled.xml")
tripinfo = ElementTree.parse("actual-tripinfo.xml")
for person, vehicle in (("by_car", "by_car_0"), ("by_taxi", "cab0")):
    ride = tripinfo.find("personinfo[@id='%s']/ride" % person)
    assert ride is not None and ride.get("vehicle") == vehicle
    assert float(ride.get("arrival")) > float(ride.get("depart"))

print("DUAROUTER and SUMO selected the expected private car and taxi.")
