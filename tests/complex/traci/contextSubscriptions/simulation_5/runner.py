#!/usr/bin/env python
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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def csRes2Str(csr):
    return ', '.join(sorted(csr[''].keys()))


def runSingle(viewRange, domain, domain2):
    name = domain._name if hasattr(domain, "_name") else domain.__name__
    name2 = domain2._name if hasattr(domain2, "_name") else domain2.__name__
    ids = domain.getIDList() if name != "simulation" else [""]
    if not ids:
        print("No objects for domain '%s' at time %s" %
              (name, traci.simulation.getTime()))
        return
    egoID = ids[0]

    print("trying to subscribe to %s around %s '%s' at time %s" % (
        name2, name, egoID, traci.simulation.getTime()))
    domain.subscribeContext(egoID, domain2.DOMAIN_ID, viewRange,
                            [traci.constants.TRACI_ID_LIST])
    responses = traci.simulationStep()
    print("   found %s objects" % len(responses))

    for i in range(3):
        print(i, csRes2Str(domain.getAllContextSubscriptionResults()))

    domain.unsubscribeContext(egoID, domain2.DOMAIN_ID, viewRange)
    responses = traci.simulationStep()
    if responses:
        print("Error: Unsubscribe did not work", responses)
    else:
        print("Ok: Unsubscribe successful")
    sys.stdout.flush()


def restart():
    traci.start([sumolib.checkBinary(sys.argv[1]),
                 '-Q', "-c", "sumo.sumocfg",
                 '-a', 'input_additional.add.xml'])
    traci.simulationStep()


#  main
restart()
for domain2 in traci.DOMAINS:
    try:
        runSingle(5, traci.simulation, domain2)
    except traci.FatalTraCIError as e:
        print("restarting sumo due to FatalTraCIError '%s'" % e)
        traci.close()
        restart()
    except traci.TraCIException as e:  # libsumo case
        print("restarting sumo due to FatalTraCIError '%s'" % e)
        traci.close()
        restart()

traci.close()
