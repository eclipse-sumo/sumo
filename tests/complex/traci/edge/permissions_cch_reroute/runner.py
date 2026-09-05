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

# @file    runner.py
# @date    2026-09-05

# Regression test for a bug where a permanent TraCI permission change
# (traci.edge.setDisallowed / traci.lane.setAllowed / setDisallowed with
# CHANGE_PERMISSIONS_PERMANENT) was silently ignored by the CCH ("--routing-
# algorithm CCH") free-flow metric once it had already been built/customized:
# MSLane::setPermissions never populated myPermissionChanges for permanent
# changes, so MSEdge::rebuildAllowedLanes's hadPermissionChanges()-based guard
# never called MSRoutingEngine::invalidateCCHEdge and a vehicle kept being
# routed across a lane/edge that TraCI had just disallowed.
#
# Network is a small diamond: OA leads into a fork at A, either directly via
# AB or via the detour AC-CB, both rejoining at B before BD leads to D.

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary('sumo'),
             '--no-step-log',
             '--routing-algorithm', 'CCH',
             '-n', 'input_net.net.xml',
             '-r', 'input_routes.rou.xml',
             ] + sys.argv[1:])

for _ in range(2):
    traci.simulationStep()

# force the free-flow CCH metric to be built/customized now, while AB is
# still allowed (routingMode stays DEFAULT so the shared metric is used
# instead of the embedded exact fallback)
traci.vehicle.rerouteTraveltime("v0", currentTravelTimes=False)
print("route while AB still allowed:", traci.vehicle.getRoute("v0"))

traci.edge.setDisallowed("AB", ["passenger"])
print("allowed on AB_0 after setDisallowed:", traci.lane.getAllowed("AB_0"))

traci.vehicle.rerouteTraveltime("v0", currentTravelTimes=False)
print("route after setDisallowed+reroute:", traci.vehicle.getRoute("v0"))

traci.simulationStep()
traci.close()
