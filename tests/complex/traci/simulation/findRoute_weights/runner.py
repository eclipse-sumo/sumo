#!/usr/bin/env python
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

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml",
             "--weight-files", "input_additional.add.xml", "--no-step-log"] + sys.argv[1:])
try:
    for departPos, arrivalPos in ((0, 100), (0, 50), (90, 100), (90, 50)):
        route = traci.simulation.findRoute("start", "end", depart=0, departPos=departPos, arrivalPos=arrivalPos)
        print("departPos=%s arrivalPos=%s travelTime=%.2f cost=%.2f length=%.2f" %
              (departPos, arrivalPos, route.travelTime, route.cost, route.length))
finally:
    traci.close()
