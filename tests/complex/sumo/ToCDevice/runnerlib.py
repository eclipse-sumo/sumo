#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runnerlib.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import optparse

# we need to import python modules from the $SUMO_HOME/tools directory
try:
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
        os.path.dirname(__file__), "..", "..", "..", "..")), "tools"))
except ImportError:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci  # noqa


def requestToC(vehID, timeTillMRM):
    traci.vehicle.setParameter(vehID, "device.toc.requestToC", str(timeTillMRM))


def printToCParams(vehID, only_dynamic=False, extra=False, add=None):
    currentAwareness = traci.vehicle.getParameter(vehID, "device.toc.currentAwareness")
    state = traci.vehicle.getParameter(vehID, "device.toc.state")
    speed = traci.vehicle.getSpeed(vehID)

    print("time", traci.simulation.getTime())
    print("ToC device infos for vehicle '%s'" % vehID)
    if not only_dynamic:
        print("Static parameters:")
        params = ["holder", "manualType", "automatedType", "responseTime", "recoveryRate",
                  "initialAwareness", "mrmDecel"]
        if extra:
            params += ["lcAbstinence", "dynamicToCThreshold", "dynamicMRMProbability",
                       "mrmKeepRight", "maxPreparationAccel"]
        if add is not None:
            params += add
        for p in params:
            print("  %s = %s" % (p, traci.vehicle.getParameter(vehID, "device.toc." + p)))
        print("Dynamic parameters:")
    print("  currentAwareness = %s" % currentAwareness)
    print("  currentSpeed = %s" % speed)
    print("  state = %s" % state)


def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("--nogui", action="store_false",
                         default=True, help="run the commandline version of sumo")
    options, args = optParser.parse_args()
    return options
