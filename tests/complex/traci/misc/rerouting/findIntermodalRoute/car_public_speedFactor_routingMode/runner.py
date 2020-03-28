#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
    "--no-step-log"]
traci.start(cmd)


traci.simulationStep()

for modes in [
    "car bicycle public",
    "public",
        ""]:
    for vType in [
            '', 'defaultCar', 'slowCar', 'slowCarFactor', 'fastCar']:
        for pType in [""]:
            ppStages("findIntermodalRoute modes='%s' vType=%s' pType='%s'" %
                     (modes, vType, pType),
                     traci.simulation.findIntermodalRoute("A0B0", "A1B1",
                                                          departPos=900,
                                                          arrivalPos=100,
                                                          pType=pType,
                                                          vType=vType,
                                                          modes=modes,
                                                          routingMode=traci.constants.ROUTING_MODE_AGGREGATED))

    print()
    print()

for modes in ["public"]:
    for vType in ['']:
        for pType in ["", "defaultPed", "slowPed", "slowPedFactor", "fastPed", "fastPedFactor"]:
            ppStages("findIntermodalRoute modes='%s' vType=%s' pType='%s'" %
                     (modes, vType, pType),
                     traci.simulation.findIntermodalRoute("A0B0", "A1B1",
                                                          departPos=900,
                                                          arrivalPos=100,
                                                          pType=pType,
                                                          vType=vType,
                                                          modes=modes,
                                                          routingMode=traci.constants.ROUTING_MODE_AGGREGATED))

    print()
    print()

traci.close()
