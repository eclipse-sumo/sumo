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

import subprocess
from xml.etree import ElementTree as ET

import sumolib


def write_xml(filename, root, schema):
    root.set("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
    root.set("xsi:noNamespaceSchemaLocation", f"http://sumo.dlr.de/xsd/{schema}_file.xsd")
    ET.ElementTree(root).write(filename, encoding="utf-8", xml_declaration=True)


nodes = ET.Element("nodes")
for index in range(5):
    ET.SubElement(nodes, "node", id=f"n{index}", x=str(100 * index), y="0")
write_xml("road.nod.xml", nodes, "nodes")
edges = ET.Element("edges")
for edge, start, end, speed in (
    ("start", 0, 1, "20"), ("first", 1, 2, "10"), ("second", 2, 3, "10"), ("end", 3, 4, "20"),
    ("fastFirst-sink", 1, 2, "40"), ("fastSecond", 2, 3, "40"),
):
    item = ET.SubElement(edges, "edge", id=edge, speed=speed, **{"from": f"n{start}", "to": f"n{end}"})
    if edge.startswith("fast"):
        item.set("shape", f"{100 * start},0 {100 * start + 50},25 {100 * end},0")
write_xml("road.edg.xml", edges, "edges")
subprocess.check_call([sumolib.checkBinary("netconvert"), "-n", "road.nod.xml", "-e", "road.edg.xml",
                       "--no-turnarounds", "--no-internal-links", "-o", "road.net.xml"],
                      stdout=subprocess.DEVNULL)

zones = ET.Element("additional")
for zone, edge in (("origin", "start"), ("district-west", "first"), ("district-east-sink", "second"), ("end", "end")):
    ET.SubElement(zones, "taz", id=zone, edges=edge)
empty = ET.SubElement(zones, "taz", id="no-sinks")
ET.SubElement(empty, "tazSource", id="first", weight="1")
write_xml("zones.add.xml", zones, "additional")

for label, waypoints in (
    ("direct", {}),
    ("legacy", {"via": "district-west-sink district-east-sink-sink"}),
    ("public", {"viaTaz": "district-west district-east-sink"}),
):
    routes = ET.Element("routes")
    trip = ET.SubElement(routes, "trip", id="t", depart="0", to="end", **{"from": "start"}, **waypoints)
    ET.SubElement(trip, "stop", lane="end_0", endPos="75", duration="1")
    ET.SubElement(routes, "flow", id="f", begin="5", end="10", number="1", fromTaz="origin", toTaz="end", **waypoints)
    write_xml(label + ".rou.xml", routes, "routes")


def routed(filename):
    return {vehicle.get("id"): vehicle.find("route").get("edges")
            for vehicle in ET.parse(filename).findall("vehicle")}


for app in ("sumo", "duarouter"):
    common = [sumolib.checkBinary(app), "-n", "road.net.xml", "-a", "zones.add.xml",
              "--xml-validation.routes", "always"]
    if app == "sumo":
        common += ["--no-step-log", "--no-duration-log", "--end", "120"]
    output_option = "--vehroute-output" if app == "sumo" else "-o"
    expected = {"t": "start first second end", "f.0": "start first second end"}
    for label in ("direct", "legacy", "public"):
        output = f"{app}-{label}.out.xml"
        subprocess.check_call(common + ["-r", label + ".rou.xml", output_option, output], stdout=subprocess.DEVNULL)
        actual = routed(output)
        if label == "direct":
            assert actual.keys() == expected.keys() and all(route != expected[vid] for vid, route in actual.items())
        else:
            assert actual == expected, (label, actual)

    for zone, message in (("missing", "Unknown intermediate TAZ 'missing'"),
                          ("fastFirst", "Unknown intermediate TAZ 'fastFirst'"),
                          ("no-sinks", "Intermediate TAZ 'no-sinks' has no incoming edges")):
        routes = ET.Element("routes")
        ET.SubElement(routes, "trip", id="invalid", depart="0", to="end", viaTaz=zone, **{"from": "start"})
        write_xml("invalid.rou.xml", routes, "routes")
        result = subprocess.run(common + ["-r", "invalid.rou.xml", output_option, "invalid.out.xml"],
                                capture_output=True, text=True, check=False)
        assert result.returncode != 0 and message in result.stderr, result.stderr

    print(f"{app} routed trip and flow through both TAZs and rejected invalid zones.")
