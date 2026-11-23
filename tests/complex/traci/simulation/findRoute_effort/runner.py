#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

import math
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
from traci import constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net4.net.xml", "--no-step-log"] + sys.argv[1:])
try:
    detour = ("A0B0", "B0B1", "B1C1", "C1C0", "C0D0")
    for edge in traci.edge.getIDList():
        if not edge.startswith(":"):
            traci.edge.setEffort(edge, 1 if edge in detour else 100)
    fastest = traci.simulation.findRoute("A0B0", "C0D0")
    assert fastest.edges == ("A0B0", "B0C0", "C0D0"), fastest
    assert fastest.cost == fastest.travelTime, fastest

    combined = traci.simulation.findRoute("A0B0", "C0D0", routingMode=tc.ROUTING_MODE_COMBINED)
    assert combined.edges == fastest.edges and combined.cost == fastest.cost, combined

    cheapest = traci.simulation.findRoute("A0B0", "C0D0", routingMode=tc.ROUTING_MODE_EFFORT)
    assert cheapest.edges == detour, cheapest
    assert cheapest.cost == 5 and cheapest.travelTime > fastest.travelTime, cheapest

    half_start = traci.lane.getLength("A0B0_0") / 2
    half_end = traci.lane.getLength("C0D0_0") / 2
    partial = traci.simulation.findRoute("A0B0", "C0D0", routingMode=tc.ROUTING_MODE_EFFORT,
                                         departPos=half_start, arrivalPos=half_end)
    assert partial.edges == detour and partial.cost == 4, partial
    assert math.isclose(partial.length, cheapest.length - half_start - half_end), partial
    assert partial.travelTime < cheapest.travelTime, partial
    print("Effort routing selected a longer route and reported effort separately from travel time.")
finally:
    traci.close()
