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
# @date    2026-11-23

import gzip
import os
from pathlib import Path
import subprocess
import sys
from xml.etree import ElementTree
import zlib

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa


def zlib_small_window(data):
    compressor = zlib.compressobj(wbits=9)
    return compressor.compress(data) + compressor.flush()


network = Path("network.net.xml")
routes = Path("routes.rou.xml")
config = Path("simulation.sumocfg")
tripinfo = Path("tripinfo.xml")
subprocess.run([checkBinary("netgenerate"), "--grid", "--grid.number", "2", "-o", str(network)],
               stdout=subprocess.DEVNULL, check=True)
network_xml = network.read_bytes()
edge = next(e.get("id") for e in ElementTree.fromstring(network_xml).findall("edge")
            if not e.get("id").startswith(":"))
routes_xml = ('<routes><vehicle id="car" depart="0"><route edges="%s"/></vehicle></routes>' % edge).encode()
config.write_text('''<configuration>
    <input><net-file value="network.net.xml"/><route-files value="routes.rou.xml"/></input>
    <time><end value="2"/></time>
    <output><tripinfo-output value="tripinfo.xml"/><tripinfo-output.write-unfinished value="true"/></output>
</configuration>''', encoding="utf-8")
sumo = checkBinary("sumo")

for label, encode in (("plain", bytes), ("gzip", gzip.compress),
                      ("zlib level 2", lambda data: zlib.compress(data, 2)),
                      ("zlib with a small window", zlib_small_window)):
    for path, data in ((network, network_xml), (routes, routes_xml)):
        path.write_bytes(encode(data))
    subprocess.run([sumo, str(network)], stdout=subprocess.DEVNULL, check=True)
    subprocess.run([sumo, "-c", str(config), "--no-step-log", "--no-duration-log"], check=True)
    car = ElementTree.parse(tripinfo).find("tripinfo")
    assert car.get("id") == "car" and float(car.get("depart")) == 0, label
    tripinfo.unlink()
    print("Loaded %s network and routes." % label)
