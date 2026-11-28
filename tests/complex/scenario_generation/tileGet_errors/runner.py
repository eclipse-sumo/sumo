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
import os
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
from threading import Thread
from types import SimpleNamespace

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import tileGet  # noqa: E402


PNG = b64decode("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMCAO+jR4kAAAAASUVORK5CYII=")


class TileServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        if self.path == "/denied":
            self.send_header("Content-Type", "image/jpeg")
            content = b"\xef\xbb\xbf  \n<html><title>Access denied</title></html>" + b" " * 4000
        else:
            self.send_header("Content-Type", "image/png")
            content = PNG
        self.end_headers()
        self.wfile.write(content)

    def log_message(self, *args):
        pass


with HTTPServer(("127.0.0.1", 0), TileServer) as server, TemporaryDirectory() as directory:
    thread = Thread(target=server.serve_forever)
    thread.start()
    try:
        url = "http://127.0.0.1:%s" % server.server_port
        tile = Path(directory) / "tile.jpeg"
        options = SimpleNamespace(simulate=False, min_file_size=0, key=None)
        try:
            tileGet.worker(options, url + "/denied", tile)
        except ValueError as error:
            assert "Access denied" in str(error), str(error)
        else:
            raise AssertionError("Downloaded an error page as a tile")
        assert not tile.exists()
        tile = Path(directory) / "tile.png"
        tileGet.worker(options, url + "/valid", tile)
        assert tile.read_bytes() == PNG
    finally:
        server.shutdown()
        thread.join()

print("Reported the tile server error and preserved the valid PNG.")
