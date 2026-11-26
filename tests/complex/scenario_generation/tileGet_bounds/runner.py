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
from concurrent.futures import ThreadPoolExecutor
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import os
import subprocess
import sys
from tempfile import TemporaryDirectory
import threading
from unittest.mock import patch
from xml.etree import ElementTree

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import tileGet  # noqa: E402

PNG = b64decode("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMCAO+jR4kAAAAASUVORK5CYII=")


class TileHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        with self.server.lock:
            self.server.requests.append(self.path)
        self.send_response(200)
        self.send_header("Content-Type", "image/png")
        self.send_header("Content-Length", str(len(PNG)))
        self.end_headers()
        self.wfile.write(PNG)

    def log_message(self, *args):
        pass


class Network:
    def getBBoxXY(self):
        return (100, 100), (200, 200)

    def convertXY2LonLat(self, x, y):
        return x / 100, y / 100

    def convertLonLat2XY(self, lon, lat):
        return lon * 100, lat * 100


with ThreadingHTTPServer(("127.0.0.1", 0), TileHandler) as http_srv, TemporaryDirectory() as directory:
    http_srv.requests = []
    http_srv.lock = threading.Lock()
    srv_thread = threading.Thread(target=http_srv.serve_forever, daemon=True)
    srv_thread.start()
    try:
        base_url = "http://127.0.0.1:%d" % http_srv.server_port
        mapserver_url = base_url + "/MapServer/tile"
        tile_tool = os.path.join(os.environ["SUMO_HOME"], "tools", "tileGet.py")

        sub1 = os.path.join(directory, "cli_bbox")
        os.makedirs(sub1)
        res = subprocess.run(
            [sys.executable, tile_tool, "--bbox", "20,10,21,11", "--zoom", "7",
             "--url", mapserver_url, "--output-dir", sub1, "--min-file-size", "0"],
            capture_output=True, text=True, check=True,
        )
        assert "No network given" in res.stderr
        start_x, start_y = tileGet.fromLatLonToTile(11, 20, 7)
        end_x, end_y = tileGet.fromLatLonToTile(10, 21, 7)
        expected_paths = ["/MapServer/tile/7/%s/%s" % (y, x)
                          for x in range(start_x, end_x + 1) for y in range(start_y, end_y + 1)]
        assert http_srv.requests == expected_paths

        with patch.object(tileGet.sumolib.net, "readNet", return_value=Network()):
            sub2 = os.path.join(directory, "decal")
            os.makedirs(sub2)
            tileGet.get(["--net", "input.net.xml", "--output-dir", sub2, "--bbox", "20,10,21,11",
                         "--url", base_url + "/staticmap", "--min-file-size", "0"])
            decal = ElementTree.parse(os.path.join(sub2, "settings.xml")).find("decal")
            assert (float(decal.get("centerX")), float(decal.get("centerY"))) == (2050.0, 1050.0)
            assert (float(decal.get("width")), float(decal.get("height"))) == (100.0, 100.0)

            http_srv.requests.clear()
            sub3 = os.path.join(directory, "bounded")
            os.makedirs(sub3)
            with patch.object(tileGet, "ThreadPoolExecutor", wraps=ThreadPoolExecutor) as pool:
                tileGet.get(["--net", "input.net.xml", "--output-dir", sub3, "--bbox=-90,-60,90,60",
                             "--zoom", "4", "--url", mapserver_url, "--parallel-jobs", "10000", "--min-file-size", "0"])
                assert len(http_srv.requests) > 32
                pool.assert_called_once_with(max_workers=32)
    finally:
        http_srv.shutdown()
        srv_thread.join()

print("Explicit bounds control tile requests and decal placement. MapServer download workers are bounded.")
