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
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402


args = ["-n", "input_net.net.xml", "--no-step-log"]
traci.start([sumolib.checkBinary("sumo")] + args)
try:
    print("No container stops", traci.simulation.getContainerStopIDList())
    traci.load(args + ["-a", "input_additional.add.xml"])
    print("Container stops", traci.simulation.getContainerStopIDList())
    print("Bus stops", traci.busstop.getIDList())
finally:
    traci.close()
