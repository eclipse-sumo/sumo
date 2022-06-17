#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import math

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

def runSingle(traciEndTime, viewRange, domain, domain2):
    seen1 = 0
    seen2 = 0
    step = 0
    subscribed = False
    ids = domain.getIDList()
    if not ids:
        print("No objects for domain '%s' at time %s" % (
            domain._name, traci.simulation.getTime()))
        return
    egoID  = ids[0]

    domain.subscribeContext(egoID, domain2.DOMAIN_ID, viewRange)
    responses = traci.simulationStep()
    print("found %s %s around %s %s at time %s" % (
        len(responses), domain2._name, domain._name, egoID,
        traci.simulation.getTime()))

    domain.unsubscribeContext(egoID, domain2.DOMAIN_ID, viewRange)
    responses = traci.simulationStep()
    if responses:
        print("Error: Unsubscribe did not work", responses)
    else:
        print("Ok: Unsubscribe successful")
    sys.stdout.flush()


traci.start([sumolib.checkBinary(sys.argv[1]), '-Q', "-c", "sumo.sumocfg",
    '-a', 'input_additional.add.xml'])
traci.simulationStep()

for domain in traci.domain._defaultDomains:
    for domain2 in traci.domain._defaultDomains:
        runSingle(2, 100, domain, domain2)

