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
# @author  Josh
# @date    2026-11-26

from base64 import b64decode
from http.server import BaseHTTPRequestHandler, HTTPServer
from io import BytesIO
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory
from threading import Thread
from xml.etree import ElementTree as ET
from zipfile import ZipFile

import os
sys.path.insert(0, os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa: E402
from osmWebWizard import Builder  # noqa: E402


OSM = b'''<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6" generator="SUMO test">
    <node id="1" lat="52.5" lon="13.4"/>
    <node id="2" lat="52.5" lon="13.404"/>
    <node id="3" lat="52.50001" lon="13.401">
        <tag k="amenity" v="taxi"/>
        <tag k="name" v="Taxi &amp; &quot;Ride&quot;"/>
        <tag k="capacity" v="4"/>
    </node>
    <node id="4" lat="52.50001" lon="13.403">
        <tag k="amenity" v="charging_station"/>
        <tag k="name" v="Charge here"/>
    </node>
    <way id="5">
        <nd ref="1"/>
        <nd ref="2"/>
        <tag k="highway" v="primary"/>
    </way>
</osm>'''


class LocalMap(BaseHTTPRequestHandler):
    def do_POST(self):
        request = ET.fromstring(self.rfile.read(int(self.headers["Content-Length"])))
        self.server.amenities = {tag.attrib["v"] for tag in request.iter("has-kv")
                                 if tag.get("k") == "amenity" and tag.get("v")}
        self.send_response(200)
        self.send_header("Content-Type", "application/xml")
        self.send_header("Content-Length", str(len(OSM)))
        self.end_headers()
        self.wfile.write(OSM)


def generate(root, port, taxi, charging):
    data = {"coords": [13.39, 52.49, 13.41, 52.51], "osmMirror": "http://127.0.0.1:%s/" % port,
            "roadTypes": {"highway": ["primary"]}, "poly": False, "publicTransport": False,
            "decal": False, "leftHand": False, "carOnlyNetwork": False, "duration": 10, "vehicles": {},
            "outputDir": str(root), "taxiStands": taxi, "chargingStations": charging}
    builder = Builder(data, True)
    builder.build()
    builder.makeConfigFile()
    builder.createBatch()
    return builder


def verify():
    with HTTPServer(("127.0.0.1", 0), LocalMap) as mirror, TemporaryDirectory() as temp:
        thread = Thread(target=mirror.serve_forever, daemon=True)
        thread.start()
        try:
            root = Path(temp)
            port = mirror.server_address[1]
            generate(root / "default", port, False, False)
            assert mirror.amenities == set()
            assert not list((root / "default").glob("*.add.xml"))

            taxi_only = generate(root / "taxi", port, True, False)
            assert mirror.amenities == {"taxi"}
            assert "taxi" in taxi_only.files and "charging_station" not in taxi_only.files

            selected = generate(root / "selected", port, True, True)
            assert mirror.amenities == {"taxi", "charging_station"}
            taxi = ET.parse(selected.files["taxi"]).getroot().find("parkingArea")
            charger = ET.parse(selected.files["charging_station"]).getroot().find("chargingStation")
            assert taxi is not None and taxi.get("roadsideCapacity") == "4"
            assert taxi.get("name") == 'Taxi & "Ride"'
            assert charger is not None and charger.get("name") == "Charge here"
            subprocess.run([sumolib.checkBinary("sumo"), "-c", selected.files["config"],
                            "--xml-validation", "always", "--end", "1"], check=True, timeout=30)
            with ZipFile(BytesIO(b64decode(selected.createZip()))) as package:
                names = {Path(name).name for name in package.namelist()}
                assert {"osm.taxi.add.xml", "osm.charging_station.add.xml"} <= names
        finally:
            mirror.shutdown()
            thread.join()


if len(sys.argv) > 1:
    verify()
else:
    result = subprocess.run([sys.executable, __file__, "--verify"], capture_output=True, text=True, timeout=90)
    if result.returncode:
        sys.stderr.write(result.stdout + result.stderr)
        sys.exit(result.returncode)
    print("Verified the Web Wizard imports and packages the selected stopping places.")
