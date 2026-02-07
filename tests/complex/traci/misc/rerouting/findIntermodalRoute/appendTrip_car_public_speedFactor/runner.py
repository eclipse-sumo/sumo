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
# @author  Jakob Erdmann
# @date    2015-02-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "..", "..", "tools"))

import traci  # noqa
import sumolib  # noqa


def ppStages(comment, stages):
    print("%s\n  %s\n" % (comment, "\n  ".join(map(str, stages))))


sumoBinary = sumolib.checkBinary('sumo')
cmd = [
    sumoBinary,
    "-n", "input_net.net.xml",
    "-a", "input_additional.add.xml,input_additional2.add.xml",
    "-r", "input_routes.rou.xml",
    "--tripinfo-output", "tripinfo.xml",
    "--vehroute-output", "vehroutes.xml",
    "--vehroute-output.dua",
    "--device.tripinfo.probability", "0",
    "--device.vehroute.probability", "0",
    "--no-step-log"]
traci.start(cmd)


traci.simulationStep()
traci.vehicle.add("taxi_0", "", "taxi")

pID = "p0"
traci.person.add(pID, "A0B0", 900)
stage = traci.simulation.Stage(
    type=traci.constants.STAGE_TRIP,
    vType="DEFAULT_BIKETYPE DEFAULT_VEHTYPE",
    line="car public", edges=[], destStop="stopB_N",
    arrivalPos=100)

traci.person.appendStage(pID, stage)

pID = "p1"
traci.person.add(pID, "A0B0", 900)
stage = traci.simulation.Stage(
    type=traci.constants.STAGE_TRIP,
    line="taxi", edges=[], destStop="stopB_N",
    arrivalPos=100)

traci.person.appendStage(pID, stage)

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()

traci.close()
