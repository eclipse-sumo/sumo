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
# @author  Michael Behrisch
# @date    2026-11-28

import base64
import gzip
import io
import json
import os
import subprocess
import sys
import time
from urllib.request import Request, urlopen
from zipfile import ZipFile

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import sumolib  # noqa


def wait_for_server(base_url, timeout=10.0):
    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            with urlopen(base_url + "/index.html", timeout=1.0) as resp:
                return resp.read().decode("utf-8")
        except Exception:
            time.sleep(0.1)
    raise RuntimeError("osmWebWizard server did not start in time")


def run_build(base_url, payload, timeout=30.0):
    req = Request(
        base_url + "/build",
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    with urlopen(req, timeout=5.0) as resp:
        assert resp.status == 200
    deadline = time.time() + timeout
    reports = []
    while time.time() < deadline:
        with urlopen(base_url + "/progress", timeout=5.0) as resp:
            msg = json.loads(resp.read().decode("utf-8")).get("data", "")
        if not msg:
            time.sleep(0.1)
            continue
        if msg.startswith("report "):
            reports.append(msg[len("report "):])
        elif msg.startswith("zip "):
            raw_zip = base64.b64decode(msg[len("zip "):])
            return reports, ZipFile(io.BytesIO(raw_zip))
    raise RuntimeError("Timed out waiting for build zip (reports=%s)" % reports)


def main():
    port = sumolib.miscutils.getFreeSocketPort()
    wizard = os.path.join(os.environ["SUMO_HOME"], "tools", "osmWebWizard.py")
    proc = subprocess.Popen(
        [sys.executable, wizard, "--remote", "--address", "127.0.0.1", "--port", str(port)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    base_url = "http://127.0.0.1:%s" % port
    try:
        html = wait_for_server(base_url)
        assert 'id="osmFile"' in html, "Missing osmFile widget in index.html"
        assert 'id="clearOsmFile"' in html, "Missing clearOsmFile widget in index.html"
        print("UI widget check passed")

        with open("osm.xml", "rb") as f:
            osm_bytes = f.read()

        base_payload = {
            "language": "en",
            "poly": False,
            "duration": 100,
            "publicTransport": False,
            "ptModes": {},
            "leftHand": False,
            "decal": False,
            "tileZoom": None,
            "carOnlyNetwork": False,
            "verbose": False,
            "roadTypes": {},
            "coords": None,
        }

        payload_xml = dict(base_payload)
        payload_xml["osmContent"] = base64.b64encode(osm_bytes).decode("ascii")
        payload_xml["osmFileName"] = "osm.xml"
        payload_xml["vehicles"] = {"bicycle": {"count": 2, "fringeFactor": 2}}
        reports_xml, zf_xml = run_build(base_url, payload_xml)
        names_xml = set(os.path.basename(n) for n in zf_xml.namelist())
        assert "osm.net.xml.gz" in names_xml, names_xml
        assert "osm.sumocfg" in names_xml, names_xml
        assert "osm.bicycle.trips.xml" in names_xml, names_xml
        print("XML build passed:", reports_xml[0])

        payload_gz = dict(base_payload)
        payload_gz["osmContent"] = base64.b64encode(gzip.compress(osm_bytes)).decode("ascii")
        payload_gz["osmFileName"] = "osm.xml.gz"
        payload_gz["vehicles"] = {}
        reports_gz, zf_gz = run_build(base_url, payload_gz)
        names_gz = set(os.path.basename(n) for n in zf_gz.namelist())
        assert "osm.net.xml.gz" in names_gz, names_gz
        assert "osm.sumocfg" in names_gz, names_gz
        print("GZ build passed:", reports_gz[0])
    finally:
        proc.terminate()
        proc.wait(timeout=5)


if __name__ == "__main__":
    main()
