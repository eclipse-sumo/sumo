#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2024 German Aerospace Center (DLR) and others.
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
# @date    2020-10-15

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net4.net.xml",
             "-r", "input_routes.rou.xml",
             "--tripinfo-output", "tripinfo.xml",
             "--no-step-log",
             ] + sys.argv[1:])

for i in range(10):
    if i == 5:
        traci.person.removeStages("0")
    for edge in traci.edge.getIDList():
        persons = traci.edge.getLastStepPersonIDs(edge)
        if persons:
            print("%s edge=%s persons=%s" % (
                traci.simulation.getTime(), edge, persons))
    traci.simulation.step()

traci.close()
