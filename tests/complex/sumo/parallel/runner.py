#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
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
# @date    2020-09-21

from __future__ import print_function
import os
import sys
import subprocess
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

numLanes = int(sys.argv[1]) if len(sys.argv) > 1 else 60
length = float(sys.argv[2]) if len(sys.argv) > 2 else 1e5
with open("%s.edg.xml" % numLanes, "w") as edges, open("%s.nod.xml" % numLanes, "w") as nodes, \
     open("%s.rou.xml" % numLanes, "w") as flows:
    print("<edges>", file=edges)
    print("<nodes>", file=nodes)
    print("<routes>", file=flows)
    for i in range(numLanes):
        print('    <node id="s%s" x="0" y="%s"/>' % (i, i*10), file=nodes)
        print('    <node id="t%s" x="%s" y="%s"/>' % (i, length, i*10), file=nodes)
        print('    <edge id="%s" from="s%s" to="t%s"/>' % (i, i, i), file=edges)
        print('    <flow id="%s" begin="0.00" end="100" period="2"><route edges="%s"/></flow>' % (i, i), file=flows)
    print("</edges>", file=edges)
    print("</nodes>", file=nodes)
    print("</routes>", file=flows)
subprocess.call([sumolib.checkBinary("netconvert"), "-o", "%s.net.xml" % numLanes, "-n", nodes.name, "-e", edges.name])
# generate config
subprocess.call([sumolib.checkBinary("sumo"), "-n", "%s.net.xml" % numLanes, "-r", flows.name, "--no-step-log",
                 "-C", "%s.sumocfg" % numLanes])
# run single threaded
subprocess.call([sumolib.checkBinary("sumo"), "-c", "%s.sumocfg" % numLanes])
# run with 2 threads
subprocess.call([sumolib.checkBinary("sumo"), "-c", "%s.sumocfg" % numLanes, "--threads", "2"])
