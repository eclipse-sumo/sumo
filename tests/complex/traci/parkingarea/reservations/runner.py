#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-04-12

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


def dump_state(label):
    print(label)
    for pa in ("pa_0", "pa_1"):
        print("  %s cap=%s occ=%s res=%s ids='%s'" % (
            pa,
            traci.parkingarea.getParameter(pa, "capacity"),
            traci.parkingarea.getParameter(pa, "occupancy"),
            traci.parkingarea.getParameter(pa, "reservations"),
            traci.parkingarea.getParameter(pa, "reservationIDs"),
        ))


traci.start([sumolib.checkBinary('sumo'),
             '-n', 'input_net2.net.xml',
             '-a', 'input_additional.add.xml',
             '-r', 'input_routes.rou.xml',
             '--no-step-log',
             ] + sys.argv[1:])

print("custom param:", traci.parkingarea.getParameter("pa_0", "custom"))
dump_state("initial:")

seen_all_reserved = False
seen_v0_parked = False
for _ in range(120):
    traci.simulationStep()
    if not seen_all_reserved and traci.parkingarea.getParameter("pa_1", "reservations") == "2":
        dump_state("after all reserved:")
        print("aliases:", traci.parkingarea.getParameter("pa_1", "reservationCount"),
              traci.parkingarea.getParameter("pa_1", "reservedVehicles"))
        seen_all_reserved = True
    if seen_all_reserved and not seen_v0_parked and traci.parkingarea.getParameter("pa_0", "occupancy") == "1":
        dump_state("after v0 parked:")
        seen_v0_parked = True
        break

traci.close()
