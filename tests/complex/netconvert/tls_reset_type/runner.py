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
edges = ET.Element("edges")
connections = ET.Element("connections")
programs = ET.Element("tlLogics")
for index, name in enumerate(("road", "other")):
    x = 500 * index
    for node, dx, y, node_type in (("center", 0, 0, "traffic_light"),
                                   ("west", -100, 0, "priority"), ("east", 100, 0, "priority"),
                                   ("south", 0, -100, "priority"), ("north", 0, 100, "priority")):
        ET.SubElement(nodes, "node", id=name if node == "center" else name + node,
                      x=str(x + dx), y=str(y), type=node_type)
    for direction, incoming, outgoing in (("horizontal", "west", "east"), ("vertical", "south", "north")):
        arrival = name + direction + "In"
        departure = name + direction + "Out"
        ET.SubElement(edges, "edge", id=arrival, **{"from": name + incoming, "to": name})
        ET.SubElement(edges, "edge", id=departure, **{"from": name, "to": name + outgoing})
        ET.SubElement(connections, "connection", **{"from": arrival, "to": departure})

for node, x, y, node_type in (
    ("r0", 900, 0, "priority"), ("railSignal", 1000, 0, "rail_signal"), ("r1", 1100, 0, "priority"),
    ("railCrossing", 1200, 0, "rail_crossing"), ("r2", 1300, 0, "priority"),
    ("c0", 1200, -100, "priority"), ("c1", 1200, 100, "priority"),
):
    ET.SubElement(nodes, "node", id=node, x=str(x), y=str(y), type=node_type)
for before, after, lane_type in (
    ("r0", "railSignal", "rail"), ("railSignal", "r1", "rail"), ("r1", "railCrossing", "rail"),
    ("railCrossing", "r2", "rail"), ("c0", "railCrossing", "passenger"), ("railCrossing", "c1", "passenger"),
):
    ET.SubElement(edges, "edge", id=before + "-" + after, allow=lane_type, **{"from": before, "to": after})

for tls, pid, controller, offset in (
    ("road", "commute", "static", "7"),
    ("road", "detectors", "actuated", "11"),
    ("other", "daily", "delay_based", "5"),
):
    program = ET.SubElement(programs, "tlLogic", id=tls, programID=pid, offset=offset, type=controller)
    for duration, state in (("18", "Gr"), ("3", "yr"), ("27", "rG"), ("3", "ry")):
        phase = ET.SubElement(program, "phase", duration=duration, state=state)
        if controller != "static" and "G" in state:
            phase.set("minDur", "10")
            phase.set("maxDur", "45")
    ET.SubElement(program, "param", key="description", value=pid)

for path, root in (("nodes.nod.xml", nodes), ("edges.edg.xml", edges),
                   ("connections.con.xml", connections), ("programs.tll.xml", programs)):
    ET.ElementTree(root).write(path)


def convert(output, options):
    subprocess.check_call([netconvert, *options, "-o", output], stdout=subprocess.DEVNULL)


def plans(path):
    root = ET.parse(path)
    return {
        (program.get("id"), program.get("programID")):
        (program.get("type"), program.get("offset"),
         [(p.get("duration"), p.get("state")) for p in program.findall("phase")],
         [parameter.attrib for parameter in program.findall("param")])
        for program in root.findall("tlLogic")
    }


convert("seed.net.xml", ("-n", "nodes.nod.xml", "-e", "edges.edg.xml", "-x", "connections.con.xml",
                         "-i", "programs.tll.xml", "--no-turnarounds"))
original = plans("seed.net.xml")
for label, options in (
    ("Explicit default only", ("--tls.default-type", "actuated")),
    ("Reset to implicit static", ("--tls.reset-type",)),
    ("Reset to actuated", ("--tls.reset-type", "--tls.default-type", "actuated")),
    ("Reset and rebuild to implicit static", ("--tls.reset-type", "--tls.rebuild")),
):
    convert("reset.net.xml", ("-s", "seed.net.xml", *options))
    reset = plans("reset.net.xml")
    assert original.keys() == reset.keys()
    types = " ".join(f"{tls}/{pid}={reset[(tls, pid)][0]}" for tls, pid in sorted(reset))
    preserved = all(original[key][1:] == reset[key][1:] for key in reset)
    root = ET.parse("reset.net.xml")
    rail = [node.get("type") for node in root.findall("junction")
            if node.get("id") in ("railSignal", "railCrossing")]
    print(label + ": " + types)
    print(f"  Same phases, offsets and parameters: {preserved}. Special junctions: {' '.join(sorted(rail))}.")

for type_name in ("railSignal", "railCrossing"):
    run = subprocess.run([netconvert, "-s", "seed.net.xml", "--tls.reset-type", "--tls.default-type", type_name,
                          "-o", "invalid.net.xml"], check=False, capture_output=True, text=True)
    assert run.returncode != 0
    assert "requires a road traffic light type" in run.stderr
    print("Rejected railway type " + type_name)
