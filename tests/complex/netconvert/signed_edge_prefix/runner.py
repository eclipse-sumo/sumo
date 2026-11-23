#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied. GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

from pathlib import Path
import subprocess
from xml.etree import ElementTree as ET

import sumolib


nodes = ET.Element("nodes")
edges = ET.Element("edges")
groups = (
    (("a", False), ("z", True)),
    (("b", False), ("-b", True)),
    (("-single", False),),
    (("c", False), ("d", False), ("e", True)),
    (("P_pre", False), ("-P_pre", True)),
)
for index, group in enumerate(groups):
    start = f"left{index}"
    end = f"right{index}"
    ET.SubElement(nodes, "node", id=start, x="0", y=str(index * 100))
    ET.SubElement(nodes, "node", id=end, x="100", y=str(index * 100))
    for edge, reverse in group:
        left, right = (end, start) if reverse else (start, end)
        ET.SubElement(edges, "edge", id=edge, **{"from": left, "to": right})
ET.ElementTree(nodes).write("nodes.nod.xml")
ET.ElementTree(edges).write("edges.edg.xml")
Path("kept.txt").write_text("edge:z\n", encoding="utf-8")
Path("stops.add.xml").write_text(
    '<additional><busStop id="stop" lane="z_0" startPos="20" endPos="40"/></additional>',
    encoding="utf-8")


def convert(name, *options):
    subprocess.check_call([sumolib.checkBinary("netconvert"), "--prefix.edge", "P_",
                           "--no-turnarounds", "--no-warnings", "-o", name, *options], stdout=subprocess.DEVNULL)
    return {edge.get("id"): (edge.get("from"), edge.get("to"))
            for edge in ET.parse(name).findall("edge[@from]")}


plain = ("-n", "nodes.nod.xml", "-e", "edges.edg.xml")
normal = convert("normal.net.xml", *plain)
assert "P_z" in normal and "P_-b" in normal and "P_-single" in normal
signed = ("--prefix.edge.signed",)
result = convert("signed.net.xml", *plain, *signed, "--ptstop-files", "stops.add.xml",
                 "--ptstop-output", "signed.stops.xml")
assert result == {
    "P_a": ("left0", "right0"), "-P_a": ("right0", "left0"),
    "P_b": ("left1", "right1"), "-P_b": ("right1", "left1"),
    "-P_single": ("left2", "right2"), "P_c": ("left3", "right3"),
    "P_d": ("left3", "right3"), "P_e": ("right3", "left3"),
    "P_pre": ("left4", "right4"), "-P_pre": ("right4", "left4"),
}
assert ET.parse("signed.stops.xml").find("busStop[@id='stop']").get("lane") == "-P_a_0"
assert convert("reimport.net.xml", "-s", "signed.net.xml", *signed) == result
kept = convert("kept.net.xml", *plain, *signed, "--kept-ids", "kept.txt")
assert kept["z"] == ("right0", "left0") and "P_a" in kept and "-P_a" not in kept
print("Signed prefixes preserve explicit signs and pair unique reverse edges.")
print("Parallel edges, protected IDs, stop lanes and repeated imports follow the signed mapping.")
