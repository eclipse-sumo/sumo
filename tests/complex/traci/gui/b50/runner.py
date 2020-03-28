#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @author  Laura Bieker
# @date    2011-07-22

from __future__ import absolute_import

import os
import subprocess
import sys
sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa
PORT = 8765

sumoBinary = os.environ.get(
    "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
netconvertBinary = os.environ.get(
    "NETCONVERT_BINARY", os.path.join(sumoHome, 'bin', 'netconvert'))

subprocess.call([netconvertBinary, "-n", "input_nodes.nod.xml",
                 "-e", "input_edges.edg.xml"], stdout=sys.stdout, stderr=sys.stderr)
p = subprocess.Popen(
    [sumoBinary, "-c", "sumo.sumocfg", "-v", "-S", "-Q", "--remote-port", str(PORT)],
    stdout=sys.stdout, stderr=sys.stderr)
traci.init(PORT)
traci.simulationStep(200000)
for i in range(10):
    for j in range(10):
        traci.gui.screenshot('View #0', "test_%s_%s.png" % (i, j))
    traci.simulationStep()
traci.close()
p.wait()
