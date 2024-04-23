#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import traci.constants as tc  # noqa
import sumolib  # noqa
from sumolib.net import Phase  # noqa
Logic = traci.trafficlight.Logic

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net2.net.xml",
             "--no-step-log",
             ] + sys.argv[1:])
tlsID = "C"


logics = [
        Logic("customActuated", tc.TRAFFICLIGHT_TYPE_ACTUATED, 0,
              [Phase(33, "GGGggrrrrrGGGggrrrrr", 5, 50),
               Phase(3,  "yyyyyrrrrryyyyyrrrrr"),
               Phase(33, "rrrrrGGGggrrrrrGGGgg", 5, 50),
               Phase(3,  "rrrrryyyyyrrrrryyyyy")]),
        Logic("customStatic", tc.TRAFFICLIGHT_TYPE_STATIC, 0,
              [Phase(33, "GGGggrrrrrGGGggrrrrr"),
               Phase(3,  "yyyyyrrrrryyyyyrrrrr"),
               Phase(33, "rrrrrGGGggrrrrrGGGgg"),
               Phase(3,  "rrrrryyyyyrrrrryyyyy")]),
        Logic("customDelayBased", tc.TRAFFICLIGHT_TYPE_DELAYBASED, 0,
              [Phase(33, "GGGggrrrrrGGGggrrrrr", 5, 50),
               Phase(3,  "yyyyyrrrrryyyyyrrrrr"),
               Phase(33, "rrrrrGGGggrrrrrGGGgg", 5, 50),
               Phase(3,  "rrrrryyyyyrrrrryyyyy")]),
        # Logic("customNEMA", tc.TRAFFICLIGHT_TYPE_NEMA, 0,
        #      [Phase(33, "GGGggrrrrrGGGggrrrrr", 5, 50),
        #       Phase(3,  "yyyyyrrrrryyyyyrrrrr"),
        #       Phase(33, "rrrrrGGGggrrrrrGGGgg", 5, 50),
        #       Phase(3,  "rrrrryyyyyrrrrryyyyy")]),
        ]

for logic in logics:
    traci.trafficlight.setProgramLogic(tlsID, logic)
    print("set logic type=%s" % traci.trafficlight.getParameter(tlsID, "typeName"))
    phase = traci.trafficlight.getPhase(tlsID)
    for step in range(100):
        traci.simulationStep()
        # print some information on phase change
        phase2 = traci.trafficlight.getPhase(tlsID)
        if phase != phase2:
            phase = phase2
            print(traci.simulation.getTime(), " phase=%s nextSwitch=%s" % (
                traci.trafficlight.getPhase(tlsID),
                traci.trafficlight.getNextSwitch(tlsID)))

traci.close()
