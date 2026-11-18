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

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-18

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

extra_args = [a for a in sys.argv[1:] if not a.startswith("--meso")]
traci.start([
    sumolib.checkBinary("sumo"),
    "-n", "input_net2.net.xml",
    "-r", "input_routes.rou.xml",
    "--device.btreceiver.range", "50",
    "--no-step-log",
] + extra_args)

traci.simulationStep()
print("initial range:", traci.vehicle.getParameter("ego", "device.btreceiver.range"))
print("offtime:", traci.vehicle.getParameter("ego", "device.btreceiver.offtime"))
print("t=1 currentlySeen:", repr(traci.vehicle.getParameter("ego", "device.btreceiver.currentlySeen")))
print("t=1 seen:", repr(traci.vehicle.getParameter("ego", "device.btreceiver.seen")))

saw_current = False
saw_past = False
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    if "ego" not in traci.vehicle.getIDList():
        break
    curr = traci.vehicle.getParameter("ego", "device.btreceiver.currentlySeen")
    past = traci.vehicle.getParameter("ego", "device.btreceiver.seen")
    if curr == "sender" and not saw_current:
        print("entered range: currentlySeen=%r seen=%r" % (curr, past))
        saw_current = True
    if saw_current and curr == "" and past == "sender" and not saw_past:
        print("left range: currentlySeen=%r seen=%r" % (curr, past))
        saw_past = True
        traci.vehicle.setParameter("ego", "device.btreceiver.range", "300")
        print("updated range:", traci.vehicle.getParameter("ego", "device.btreceiver.range"))
        traci.simulationStep()
        if "ego" in traci.vehicle.getIDList() and "sender" in traci.vehicle.getIDList():
            print("re-entered after range increase:",
                  repr(traci.vehicle.getParameter("ego", "device.btreceiver.currentlySeen")))
        break

traci.close()
