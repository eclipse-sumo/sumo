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


netconvert = sumolib.checkBinary("netconvert")
nodes = ET.Element("nodes")
for node, x, y in (("west", -100, 0), ("middle", 0, 10), ("branch", 100, 0),
                   ("east", 200, 0), ("north", 100, 100)):
    ET.SubElement(nodes, "node", id=node, x=str(x), y=str(y), type="priority")
edges = ET.Element("edges")
for name, start, end in (("a", "west", "middle"), ("b", "middle", "branch"),
                         ("c", "branch", "east"), ("d", "branch", "north")):
    for edge, left, right in ((name, start, end), (name + "Reverse", end, start)):
        ET.SubElement(edges, "edge", id=edge, sidewalkWidth="2", speed="13.89", priority="1",
                      **{"from": left, "to": right})
connections = ET.Element("connections")
ET.SubElement(connections, "crossing", node="middle", edges="a aReverse", priority="true")
ET.SubElement(connections, "crossing", node="branch", edges="b bReverse", priority="true")
for name, root in (("road.nod.xml", nodes), ("road.edg.xml", edges), ("road.con.xml", connections)):
    ET.ElementTree(root).write(name)
nodes.find("node[@id='middle']").set("type", "traffic_light")
ET.ElementTree(nodes).write("signalized.nod.xml")

plain = ("-n", "road.nod.xml", "-e", "road.edg.xml", "-x", "road.con.xml")
remove = ("--geometry.remove", "--geometry.remove.ignore-crossings")


def convert(output, options):
    subprocess.check_call([netconvert, *options, "-o", output], stdout=subprocess.DEVNULL)


convert("seed.net.xml", plain)
convert("signalized.net.xml", ("-n", "signalized.nod.xml", *plain[2:]))
for label, options in (
    ("Default geometry removal", (*plain, "--geometry.remove")),
    ("Ignore crossings alone", (*plain, "--geometry.remove.ignore-crossings")),
    ("Plain input with both options", (*plain, *remove)),
    ("SUMO input with both options", ("-s", "seed.net.xml", *remove)),
    ("Protected edge", (*plain, *remove, "--geometry.remove.keep-edges.explicit", "a")),
    ("Signal-controlled crossing", ("-s", "signalized.net.xml", *remove, "--tls.discard-simple")),
):
    convert("output.net.xml", options)
    root = ET.parse("output.net.xml")
    by_id = {element.get("id"): element for element in root.findall("edge")}
    for crossing in root.findall("edge[@function='crossing']"):
        assert all(edge in by_id for edge in crossing.get("crossingEdges").split())
    middle = root.find("junction[@id='middle']")
    if middle is None:
        assert len(by_id["a"].get("shape").split()) >= 3
        assert root.find("edge[@id='b']") is None
        status = "joined at middle"
    else:
        status = "middle " + middle.get("type")
    retained = sorted(c.get("id") + " " + " ".join(sorted(c.get("crossingEdges").split()))
                      for c in root.findall("edge[@function='crossing']"))
    print(label + ": " + status + ", crossings " + "; ".join(retained))
