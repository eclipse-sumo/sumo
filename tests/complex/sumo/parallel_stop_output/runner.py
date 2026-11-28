#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
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
from pathlib import Path
import subprocess
import sys
from xml.etree import ElementTree as ET

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


def run(binary, *args):
    subprocess.run([binary, *args], check=True, capture_output=True, timeout=30)


def write_fixture():
    nodes = ET.Element("nodes")
    edges = ET.Element("edges")
    routes = ET.Element("routes")
    ET.SubElement(routes, "vType", id="car", sigma="0")
    for number in range(168):
        edge = "e%03d" % number
        start = "start_" + edge
        finish = "finish_" + edge
        x = str(number * 50)
        ET.SubElement(nodes, "node", id=start, x=x, y="0")
        ET.SubElement(nodes, "node", id=finish, x=x, y="220")
        ET.SubElement(edges, "edge", attrib={"id": edge, "from": start, "to": finish,
                                             "numLanes": "1", "speed": "13.9"})
        vehicle = ET.SubElement(routes, "vehicle", id="v%03d" % number, type="car", depart="0")
        ET.SubElement(vehicle, "route", edges=edge)
        ET.SubElement(vehicle, "stop", lane=edge + "_0", endPos="75", duration="2")
    for filename, root in (("network.nod.xml", nodes), ("network.edg.xml", edges), ("routes.rou.xml", routes)):
        ET.ElementTree(root).write(filename, encoding="utf-8")


def read_stops(binary, name, threads):
    output = Path(name + ".xml")
    run(binary, "-n", "network.net.xml", "-r", "routes.rou.xml",
        "--stop-output", str(output), "--threads", str(threads), "--end", "90", "--no-step-log")
    records = [stop.attrib for stop in ET.parse(output).findall("stopinfo")]
    assert len(records) == 168, (name, len(records))
    assert len({record["id"] for record in records}) == 168
    return records


def stop_order(record):
    return float(record["ended"]), record["id"]


write_fixture()
run(sumolib.checkBinary("netconvert"), "-n", "network.nod.xml", "-e", "network.edg.xml", "-o", "network.net.xml")
sumo = sumolib.checkBinary("sumo")
serial = sorted(read_stops(sumo, "serial", 1), key=stop_order)
for threads in (2, 4):
    for repeat in range(2):
        records = read_stops(sumo, "parallel_%s_%s" % (threads, repeat), threads)
        assert records == serial, (threads, repeat)
print("All serial and parallel stop records match.")
