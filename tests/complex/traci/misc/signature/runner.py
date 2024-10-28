#!/usr/bin/env python
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
# @date    2023-11-14


import os
import sys
import inspect
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
import traci
if sys.version_info[0] > 2:
    import libsumo
else:
    def libsumo(): return None
    libsumo.DOMAINS = []

VERBOSE = False

for dt in traci.DOMAINS:
    for dl in libsumo.DOMAINS:
        if dt.DOMAIN_ID == dl.DOMAIN_ID:
            for ft in inspect.getmembers(dt):
                if ft[0][0] != "_" and inspect.ismethod(ft[1]):
                    if VERBOSE:
                        print("checking", dt._name, ft[0])
                    for fl in inspect.getmembers(dl):
                        if fl[0] == ft[0]:
                            sigt = inspect.signature(ft[1])
                            sigl = inspect.signature(fl[1])
                            if VERBOSE:
                                print("checking", sigt, sigl)
                            if sigt != sigl:
                                params = list(sigl.parameters.values())
                                if not params or params[0].kind != inspect.Parameter.VAR_POSITIONAL:
                                    print(".".join([dt._name, ft[0]]), "traci:", sigt, "libsumo:", sigl)
if not traci.isLibsumo():
    traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg"])
    traci.simulationStep()
    traci.vehicle.setLaneChangeMode("horiz", lcm=0)
    traci.vehicle.setParameter(objectID="horiz", key="blub", value="blubber")
    traci.vehicle.setParameter(objID="horiz", key="blub", value="blubber")
    traci.vehicle.setParameter(objectID="horiz", param="blub", value="blubber")
    traci.vehicle.setParameter(objID="horiz", param="blub", value="blubber")
    try:
        traci.vehicle.setParameter(oID="horiz", param="blub", value="blubber")
    except TypeError as e:
        print(e)
    traci.close()
