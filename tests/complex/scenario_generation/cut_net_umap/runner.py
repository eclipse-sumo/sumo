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
# @author  Mark Colley
# @date    2026-11-25

from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import os
from pathlib import Path
import subprocess
import sys
from threading import Thread
import xml.etree.ElementTree as ET

sumo_home = Path(os.environ["SUMO_HOME"])
network = Path("input_net.net.xml")
script = [sys.executable, str(sumo_home / "tools/net/cut_net.py"), "--no-warnings"]
command = script + ["-n", str(network)]
ring = [[12.02, 48.08], [12.07, 48.08], [12.07, 48.13], [12.02, 48.13], [12.02, 48.08]]
polygon = {"type": "Feature", "geometry": {"type": "Polygon", "coordinates": [ring]}, "properties": {}}
geojson = {"type": "FeatureCollection", "features": [polygon]}
outside = {"type": "Feature", "geometry": {"type": "Polygon",
                                           "coordinates": [[[0, 0], [1, 0], [1, 1], [0, 1], [0, 0]]]},
           "properties": {}}


class MapHandler(BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def do_GET(self):
        if self.path == "/m/12/":
            self.send_response(302)
            self.send_header("Location", "/fr/map/study-area_12")
            self.end_headers()
        elif self.path == "/fr/map/study-area_12":
            self.send_response(200)
            self.end_headers()
        elif self.path in ("/map/12/download/", "/map/13/download/", "/map/14/download/"):
            features = {"/map/12/download/": [polygon], "/map/13/download/": [],
                        "/map/14/download/": [outside]}[self.path]
            layer = {"type": "FeatureCollection", "features": features}
            group = {"type": "FeatureCollection", "features": [], "layers": [layer]}
            payload = json.dumps({"type": "umap", "layers": [group]}).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(payload)
        else:
            self.send_error(404)


def cut(output, *source):
    subprocess.check_call(command + ["-o", output] + list(source), stdout=subprocess.DEVNULL)
    return ET.parse(output).getroot()


Path("boundary.geojson").write_text(json.dumps(geojson), encoding="utf-8")
baseline = cut("local.net.xml", "--geojson", "boundary.geojson")
assert [edge.get("id") for edge in baseline.findall("edge")] == ["o"]

with ThreadingHTTPServer(("127.0.0.1", 0), MapHandler) as server:
    server_thread = Thread(target=server.serve_forever, daemon=True)
    server_thread.start()
    try:
        origin = "http://127.0.0.1:%s" % server.server_port
        view = cut("view.net.xml", "--umap-url", origin + "/en/map/study-area_12#15/52.43/13.53")
        short = cut("short.net.xml", "--umap-url", origin + "/m/12/")
        assert ET.tostring(view) == ET.tostring(baseline) == ET.tostring(short)

        original = Path("source.xml")
        original_bytes = network.read_bytes()
        original.write_bytes(original_bytes)
        subprocess.check_call(script + ["-n", str(original), "--umap-url", origin + "/en/map/study-area_12"],
                              stdout=subprocess.DEVNULL)
        assert original.read_bytes() == original_bytes
        assert ET.tostring(ET.parse("source.xml_cut.net.xml").getroot()) == ET.tostring(baseline)

        failure = subprocess.run(command + ["-o", "empty.net.xml", "--umap-url", origin + "/en/map/empty_13"],
                                 capture_output=True, text=True)
        assert failure.returncode and "No Polygon or MultiLineString boundary found" in failure.stderr
        assert not Path("empty.net.xml").exists()

        rejected = subprocess.run(command + ["-o", "outside.net.xml", "--umap-url", origin + "/en/map/outside_14"],
                                  capture_output=True, text=True)
        assert rejected.returncode and "No edges loaded" in rejected.stderr
        assert not Path("outside.net.xml").exists()
    finally:
        server.shutdown()
        server_thread.join()

print("The uMap view and short links produce the same cut as local GeoJSON.")
