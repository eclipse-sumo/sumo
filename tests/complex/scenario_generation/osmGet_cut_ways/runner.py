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

from http.server import BaseHTTPRequestHandler, HTTPServer
import os
import subprocess
import sys
import threading
import xml.etree.ElementTree as ET

OSM_PAYLOAD = (
    b'<?xml version="1.0" encoding="UTF-8"?>'
    b'<osm version="0.6">'
    b'<node id="n1" lon="-5.0" lat="5.0"/>'
    b'<node id="n2" lon="5.0" lat="5.0"/>'
    b'<node id="n3" lon="15.0" lat="5.0"/>'
    b'<way id="w1"><nd ref="n1"/><nd ref="n2"/><nd ref="n3"/><tag k="highway" v="primary"/></way>'
    b'</osm>'
)


class OverpassHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers.get("Content-Length", 0))
        if length:
            self.rfile.read(length)
        self.send_response(200)
        self.send_header("Content-Type", "application/xml")
        self.send_header("Content-Length", str(len(OSM_PAYLOAD)))
        self.end_headers()
        self.wfile.write(OSM_PAYLOAD)

    def log_message(self, *args):
        pass


with HTTPServer(("127.0.0.1", 0), OverpassHandler) as server:
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    try:
        url = "http://127.0.0.1:%s/api/interpreter" % server.server_port
        osm_get = os.path.join(os.environ["SUMO_HOME"], "tools", "osmGet.py")
        subprocess.check_call([sys.executable, osm_get, "-u", url, "-b", "0,0,10,10", "--cut-ways"])
    finally:
        server.shutdown()
        thread.join()

root = ET.parse("osm_bbox.osm.xml").getroot()
nodes = sorted(
    (n.get("id"), round(float(n.get("lon")), 2), round(float(n.get("lat")), 2))
    for n in root.findall("node")
)
way = root.find("way")
print(nodes, len(way.findall("nd")))
