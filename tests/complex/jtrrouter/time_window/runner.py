#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

import os
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa

with tempfile.TemporaryDirectory() as directory:
    net = os.path.join(directory, "net.net.xml")
    routes = os.path.join(directory, "input.rou.xml")
    output = os.path.join(directory, "output.rou.xml")
    with open(net, "w") as stream:
        stream.write('''<net version="1.20">
<edge id="e" from="a" to="b" priority="1">
    <lane id="e_0" index="0" speed="13.89" length="100" shape="0,0 100,0"/>
</edge>
<junction id="a" type="dead_end" x="0" y="0" incLanes="" intLanes="" shape="0,0"/>
<junction id="b" type="dead_end" x="100" y="0" incLanes="e_0" intLanes="" shape="100,0"/>
</net>''')
    for binary in ("duarouter", "jtrrouter"):
        demand = [('<trip id="v" depart="10" from="e" to="e"/>', 'vehicle'),
                  ('<flow id="f" begin="10" end="11" number="1" from="e" to="e"/>', 'vehicle')]
        if binary == "duarouter":
            demand += [('<person id="p" depart="10"><walk edges="e"/></person>', 'person'),
                       ('<personFlow id="p" begin="10" end="11" number="1">'
                        '<walk edges="e"/></personFlow>', 'person')]
        args = [checkBinary(binary), "-n", net, "-r", routes, "-o", output,
                "--no-step-log", "--xml-validation", "never"]
        if binary == "jtrrouter":
            args.append("--sinks=e")
        for definition, tag in demand:
            with open(routes, "w") as stream:
                stream.write("<routes>" + definition + "</routes>")
            for begin, end, excluded in ((20, 30, True), (0, 10, True), (10, 11, False)):
                result = subprocess.run(args + ["--begin", str(begin), "--end", str(end)],
                                        capture_output=True, text=True)
                assert result.returncode == 0, result.stderr
                assert ("No routes found between begin time" in result.stderr) == excluded, result.stderr
                assert bool(ET.parse(output).getroot().findall(tag)) != excluded, result.stdout
        with open(routes, "w") as stream:
            stream.write("<routes/>")
        result = subprocess.run(args, capture_output=True, text=True)
        assert result.returncode != 0, result.stdout
        assert "No routes found between begin time" not in result.stderr, result.stderr
        print(binary + ": time-window checks passed")
