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

# @file   runner.py
# @author Robert Hilbrich
# @date   2026-11-05

# A person departs on an edge whose only sidewalk is closed by a rerouter.
# The pedestrian model refuses the person and the person is deleted while it
# departs. This must not crash (libsumo state listener used a dangling pointer).

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "-a", "input_additional.add.xml",
             "--ignore-route-errors",
             "--no-step-log"] + sys.argv[1:])

for step in range(12):
    traci.simulationStep()
    departed = traci.simulation.getDepartedPersonIDList()
    arrived = traci.simulation.getArrivedPersonIDList()
    if departed or arrived:
        print("step=%s departed=%s arrived=%s" % (step, departed, arrived))
print("persons left:", traci.person.getIDList())
traci.close()
