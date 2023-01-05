#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


def print_prior_plan(personID, comment=""):
    print("prior stages for '%s' %s" % (personID, comment))
    stages = []
    i = -1
    while True:
        try:
            stages.append((i, traci.person.getStage(personID, i)))
            i -= 1
        except traci.TraCIException:
            break
    stages.reverse()
    for i, stage in stages:
        print("  %s: %s" % (i, stage))


traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net2.net.xml",
             "-r", "input_routes.rou.xml",
             "--tripinfo-output", "tripinfo.xml",
             # "--vehroute-output", "vehroutes.xml",
             "--no-step-log"] + sys.argv[1:])

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    t = traci.simulation.getTime()
    if t % 10 == 0:
        print("step", t)
        print_prior_plan("p0")

traci.close()
