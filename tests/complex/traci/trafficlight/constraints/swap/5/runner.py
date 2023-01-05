#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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
# @date    2021-04-12

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             '-n', 'input_net.net.xml',
             '-a', 'input_additional.add.xml',
             '-r', 'input_routes.rou.xml',
             '--tripinfo-output', 'tripinfo.xml',
             '--no-step-log',
             ] + sys.argv[1:])

print("constraints before swap")
for tlsID in traci.trafficlight.getIDList():
    for c in traci.trafficlight.getConstraints(tlsID):
        print(" tls=%s %s" % (tlsID, c))

newC = traci.trafficlight.swapConstraints("A", "t2", "D", "tx")
print("constraints added to avoid deadlock")
for c in newC:
    print(" tls=%s %s" % (tlsID, c))


print("constraints after swap")
for tlsID in traci.trafficlight.getIDList():
    for c in traci.trafficlight.getConstraints(tlsID):
        print(" tls=%s %s" % (tlsID, c))

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()

traci.close()
