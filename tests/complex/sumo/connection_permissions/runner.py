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

from pathlib import Path
import os
import subprocess
import sys
import xml.etree.ElementTree as ET

home = Path(os.environ["SUMO_HOME"])
sys.path.append(str(home / "tools"))
import sumolib  # noqa: E402

sumo = sumolib.checkBinary("sumo")


def simulate(network, edges, vclass, *options):
    Path("routes.rou.xml").write_text(
        '<routes><vType id="type" vClass="%s"/><vehicle id="v" type="type" depart="0">'
        '<route edges="%s"/></vehicle></routes>' % (vclass, edges),
        encoding="utf-8",
    )
    return subprocess.run(
        [sumo, "-n", str(network), "-r", "routes.rou.xml", "--tripinfo-output", "arrival.xml",
         "--no-step-log", "--no-duration-log", "--end", "120", *options],
        capture_output=True, text=True,
    )


fixtures = home / "tests/duarouter/bugs"
for directory, edge, options in (
    ("ticket15922", "E1", ()),
    ("ticket15883", "E5", ("--no-internal-links",)),
    ("ticket15883", "E5", ()),
):
    # Reuse networks already checked in for connection permission tests in duarouter.
    network = fixtures / directory / "input_net.net.xml"
    edges = "E0 " + edge
    rejected = simulate(network, edges, "passenger", *options)
    assert rejected.returncode != 0, (directory, options, rejected)
    assert "No connection between edge 'E0' and edge '%s'." % edge in rejected.stderr, rejected.stderr
    accepted = simulate(network, edges, "bus", *options)
    assert accepted.returncode == 0, accepted.stderr
    trip = ET.parse("arrival.xml").find("tripinfo[@id='v']")
    assert trip is not None and trip.get("arrivalLane") == edge + "_0"
    print(directory, list(options), "passenger rejected and bus arrived")

# A passenger trip with a flexible route can still reach E1 via the legal detour.
Path("routes.rou.xml").write_text(
    '<routes><trip id="detour" depart="0" from="E0" to="E1"/></routes>', encoding="utf-8"
)
network = fixtures / "ticket15922" / "input_net.net.xml"
routed = subprocess.run(
    [sumo, "-n", str(network), "-r", "routes.rou.xml", "--vehroute-output", "routes.xml",
     "--no-step-log", "--no-duration-log", "--end", "120"],
    capture_output=True, text=True,
)
assert routed.returncode == 0, routed.stderr
route = ET.parse("routes.xml").find("vehicle[@id='detour']/route")
assert route is not None and route.get("edges") == "E0 E2 E3 E4 E5 E1"
print("passenger trip used legal detour")
