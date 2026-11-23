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

import subprocess
from xml.etree import ElementTree as ET

import sumolib


def run(label, gap=30, platform_length=260, start="70", end="180", bidi=False, signals=True,
        branch=False, uncontrolled=(), deleted=(), options=()):
    positions = {"a": -200, "sa": 0, "b": gap, "c": gap + platform_length,
                 "sd": 2 * gap + platform_length, "z": 2 * gap + platform_length + 200}
    nodes = ET.Element("nodes")
    for node, x in positions.items():
        node_type = "rail_signal" if signals and node in ("sa", "sd") else "priority"
        ET.SubElement(nodes, "node", id=node, x=str(x), y="0", type=node_type)

    edges = ET.Element("edges")
    for edge, left, right in (("first", "a", "sa"), ("approach", "sa", "b"), ("platform", "b", "c"),
                              ("departure", "c", "sd"), ("last", "sd", "z")):
        ET.SubElement(edges, "edge", id=edge, allow="rail", speed="30", spreadType="center",
                      **{"from": left, "to": right})
        if bidi:
            ET.SubElement(edges, "edge", id="-" + edge, allow="rail", speed="30", spreadType="center",
                          **{"from": right, "to": left})
    if branch:
        ET.SubElement(nodes, "node", id="branch", x=str(positions["c"] + 30), y="5")
        ET.SubElement(edges, "edge", id="branch", allow="rail", speed="30", spreadType="center",
                      **{"from": "c", "to": "branch"})

    connections = ET.Element("connections")
    for incoming, outgoing in uncontrolled:
        ET.SubElement(connections, "connection", fromLane="0", toLane="0", uncontrolled="true",
                      **{"from": incoming, "to": outgoing})
    for incoming, outgoing in deleted:
        ET.SubElement(connections, "delete", **{"from": incoming, "to": outgoing})
    stops = ET.Element("additional")
    ET.SubElement(stops, "trainStop", id="stop", lane="platform_0", startPos=start, endPos=end)
    for filename, root in (("nodes.nod.xml", nodes), ("edges.edg.xml", edges),
                           ("connections.con.xml", connections), ("platforms.xml", stops)):
        ET.ElementTree(root).write(filename)

    subprocess.check_call([sumolib.checkBinary("netconvert"), "-n", "nodes.nod.xml", "-e", "edges.edg.xml",
                           "-x", "connections.con.xml", "--ptstop-files", "platforms.xml",
                           "--railway.signal.guess.by-stops", "-o", "output.net.xml", *options],
                          stdout=subprocess.DEVNULL)
    net = ET.parse("output.net.xml")
    ids = sorted(node.get("id") for node in net.findall("junction[@type='rail_signal']"))
    print(label + ": " + " ".join(ids))


run("Nearby signals")
run("At minimum spacing", gap=200)
run("Bidirectional corridor", bidi=True)
run("Switch", branch=True)
run("No existing signals", signals=False)
run("Uncontrolled direction", bidi=True, uncontrolled=(("-last", "-departure"),))
run("Deleted continuation", deleted=(("first", "approach"),))
run("Split locations", platform_length=800, start="300", end="580",
    options=("--railway.signal.guess.by-stops.split", "--osm.stop-output.length.train", "350"))
run("Split locations with default spacing", platform_length=800, start="300", end="580",
    options=("--railway.signal.guess.by-stops.split",))
