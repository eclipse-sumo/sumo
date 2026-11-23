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


from math import cos, pi, sin
from pathlib import Path
import subprocess
from xml.etree import ElementTree as ET

import sumolib

# The independently spaced rings have identical geometry and radial approaches.
cases = (
    ("untagged", None, None, ()),
    ("circular", "circular", None, ()),
    ("ramp", None, "primary_link", ()),
    ("service", None, "service", ()),
    ("all_signals", None, None, range(8)),
    ("some_signals", None, None, (0,)),
    ("tagged", "roundabout", "primary_link", range(8)),
)

osm = ET.Element("osm", version="0.6")
ways = []
for index, (name, junction, highway, signals) in enumerate(cases, 1):
    start = 1000 * index
    for i in range(8):
        angle = i * pi / 4
        for offset, radius in ((0, 0.001), (20, 0.0025)):
            node = ET.SubElement(osm, "node", id=str(start + i + offset),
                                 lat=f"{52 + radius * sin(angle):.8f}",
                                 lon=f"{13 + 0.01 * index + radius * cos(angle) / cos(52 * pi / 180):.8f}")
            if offset == 0 and i in signals:
                ET.SubElement(node, "tag", k="highway", v="traffic_signals")

        ring = ET.Element("way", id=str(start + 100 + i))
        ET.SubElement(ring, "nd", ref=str(start + i))
        ET.SubElement(ring, "nd", ref=str(start + (i + 1) % 8))
        ET.SubElement(ring, "tag", k="highway", v=highway if i == 1 and highway else "residential")
        if junction == "roundabout" or (junction == "circular" and i == 1):
            ET.SubElement(ring, "tag", k="junction", v=junction)
        else:
            ET.SubElement(ring, "tag", k="oneway", v="yes")
        ways.append(ring)

        arm = ET.Element("way", id=str(start + 120 + i))
        ET.SubElement(arm, "nd", ref=str(start + 20 + i))
        ET.SubElement(arm, "nd", ref=str(start + i))
        ET.SubElement(arm, "tag", k="highway", v="service" if name == "untagged" and i == 0 else "residential")
        ways.append(arm)

osm.extend(ways)
Path("rings.osm.xml").write_bytes(ET.tostring(osm, encoding="utf-8", xml_declaration=True))
netconvert = sumolib.checkBinary("netconvert")
for label, source, guess, output in (
    ("Guessing true", ("--osm-files", "rings.osm.xml"), "true", "rings.net.xml"),
    ("Reimport with guessing", ("--sumo-net-file", "rings.net.xml"), "true", "reimport.net.xml"),
    ("Guessing false", ("--osm-files", "rings.osm.xml"), "false", "tagged.net.xml"),
):
    subprocess.check_call([netconvert, *source, "--no-internal-links",
                           "--roundabouts.guess", guess, "-o", output], stdout=subprocess.DEVNULL)
    roundabouts = {int(r.get("edges").split()[0]) // 1000 for r in ET.parse(output).findall("roundabout")}
    included = [case[0] for i, case in enumerate(cases, 1) if i in roundabouts]
    print("%s: %s" % (label, ", ".join(included)))
