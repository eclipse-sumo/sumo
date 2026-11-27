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
# @date    2026-11-27

import glob
import os
import sys
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import tileGet  # noqa


class TileHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        payload = b"\xff\xd8\xff\xe0" + b"0" * 4096
        self.send_response(200)
        self.send_header("Content-Type", "image/jpeg")
        self.send_header("Content-Length", str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    def log_message(self, format, *args):
        pass


def run_in_thread(args):
    errors = []

    def target():
        try:
            tileGet.get(args)
        except Exception as exc:
            errors.append(exc)

    thread = threading.Thread(target=target)
    thread.start()
    thread.join()
    if errors:
        raise errors[0]


def verify_output(out_dir):
    tiles = sorted(glob.glob(os.path.join(out_dir, "*.jpeg")))
    assert tiles and all(os.stat(t).st_size >= 3000 for t in tiles), "missing or small tiles"
    with open(os.path.join(out_dir, "settings.xml"), encoding="utf-8") as f:
        content = f.read()
    assert "</viewsettings>" in content, "incomplete settings file"
    return len(tiles)


server = ThreadingHTTPServer(("127.0.0.1", 0), TileHandler)
server_thread = threading.Thread(target=server.serve_forever, daemon=True)
server_thread.start()
base_url = "http://127.0.0.1:%d/MapServer/tile" % server.server_address[1]

failed = False
for label, extra in (("default", []), ("parallel", ["-j", "4"])):
    out_dir = "out_%s" % label
    os.makedirs(out_dir, exist_ok=True)
    args = [
        "-b", "13.381,52.512,13.389,52.518",
        "-u", base_url,
        "-t", "4",
        "-d", out_dir,
        "-s", "settings.xml",
    ] + extra
    try:
        run_in_thread(args)
        tiles = verify_output(out_dir)
        print("%s: ok, tiles=%d" % (label, tiles))
    except Exception as exc:
        print("%s: %s: %s" % (label, type(exc).__name__, exc))
        failed = True

server.shutdown()
server.server_close()
if failed:
    sys.exit(1)
