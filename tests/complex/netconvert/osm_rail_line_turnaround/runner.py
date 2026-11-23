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

from pathlib import Path
import subprocess
from xml.etree import ElementTree as ET

import sumolib


netconvert = sumolib.checkBinary("netconvert")
oneway = ET.parse("osm.xml")
for way in oneway.findall("way"):
    way.remove(way.find("tag[@k='railway:bidirectional']"))
oneway.write("oneway.osm.xml")
Path("explicit.con.xml").write_text(
    '<connections><connection from="20" to="-20" fromLane="0" toLane="0" speed="4.5"/>'
    '<connection from="20" to="30" fromLane="0" toLane="0"/></connections>')
Path("excluded.con.xml").write_text(
    '<connections><delete from="20" to="-20"/></connections>')
for label, osm, options in (
    ("OSM line", "osm.xml", ()),
    ("Turnarounds disabled", "osm.xml", ("--no-turnarounds",)),
    ("All stops", "osm.xml", ("--railway.topology.repair.stop-turn",)),
    ("Explicit connection", "osm.xml", ("--connection-files", "explicit.con.xml")),
    ("Explicit deletion", "osm.xml", ("--connection-files", "excluded.con.xml")),
    ("One-way tracks", "oneway.osm.xml", ()),
    ("One-way tracks made bidirectional", "oneway.osm.xml", ("--railway.topology.all-bidi",)),
):
    subprocess.check_call([netconvert, "--osm-files", osm, "--ptstop-output", "line_stops.xml",
                           "--ptline-output", "lines.xml", "-o", "net.xml", *options],
                          stdout=subprocess.DEVNULL)
    print(label)
    if label in ("OSM line", "One-way tracks", "One-way tracks made bidirectional"):
        for line in ET.parse("lines.xml").findall("ptLine"):
            print("  Line %s: %s" % (line.get("id"), line.find("route").get("edges")))
    for connection in ET.parse("net.xml").findall("connection"):
        edge = connection.get("from")
        reverse = connection.get("to")
        if (edge, reverse) in (("10", "-10"), ("20", "-20"), ("-20", "20")):
            print("  Connection %s to %s at %s m/s" % (edge, reverse, connection.get("speed")))
