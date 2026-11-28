#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @author  Mark Colley
# @date    2026-11-21

import os
import subprocess
import sys
import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

with open("types.typ.xml", "w", encoding="utf-8") as f:
    f.write('<types>'
            '<type id="highway.primary" speed="20.0">'
            '<restriction vClass="passenger" speed="20.0"/>'
            '<restriction vClass="tram" speed="15.0"/>'
            '</type>'
            '<type id="railway.tram" speed="25.0">'
            '<restriction vClass="passenger" speed="25.0"/>'
            '<restriction vClass="tram" speed="22.0"/>'
            '</type>'
            '</types>')

with open("in.osm.xml", "w", encoding="utf-8") as f:
    f.write('<?xml version="1.0" encoding="UTF-8"?>'
            '<osm version="0.6">'
            '<node id="1" lat="52.50" lon="13.40"/>'
            '<node id="2" lat="52.50" lon="13.41"/>'
            '<node id="3" lat="52.51" lon="13.41"/>'
            '<way id="10"><nd ref="1"/><nd ref="2"/>'
            '<tag k="highway" v="primary"/><tag k="oneway" v="yes"/></way>'
            '<way id="11"><nd ref="2"/><nd ref="3"/>'
            '<tag k="highway" v="primary"/><tag k="railway" v="tram"/>'
            '<tag k="oneway" v="yes"/></way>'
            '</osm>')

subprocess.check_call([
    sumolib.checkBinary("netconvert"),
    "-t", "types.typ.xml",
    "--osm-files", "in.osm.xml",
    "-o", "out.net.xml",
    "--no-warnings",
])

for t in ET.parse("out.net.xml").findall("type"):
    restr = [(r.get("vClass"), r.get("speed")) for r in t.findall("restriction")]
    print(t.get("id"), t.get("speed"), restr)
