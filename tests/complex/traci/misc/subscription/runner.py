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

# @file    runner.py
# @author  Michael Behrisch
# @date    2023-11-14


import os
import sys
import inspect
from pprint import pprint

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
if os.environ.pop('LIBSUMO_AS_TRACI', None):
    import traci as pure_traci
    import libsumo as traci
elif os.environ.pop('LIBTRACI_AS_TRACI', None):
    import traci as pure_traci
    import libtraci as traci
else:
    import traci
    pure_traci = traci

try:
    traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg", "+a", "input_additional.add.xml"])
    traci.simulationStep()
    for dt in pure_traci.DOMAINS:
        if dt._name == "gui":
            continue
        for ft in inspect.getmembers(dt):
            if inspect.ismethod(ft[1]):
                source = inspect.getsource(ft[1])
                if "self._getUniversal(" in source:
                    s = source.index("_getUniversal(") + 17
                    if "," in source[s:]:
                        variable = source[s:source.index(",", s)]
                        remainder = source[source.index(",", s):source.index(")", s)]
                    else:
                        variable = source[s:source.index(")", s)]
                        remainder = ""
                    if hasattr(traci.constants, variable):
                        print("Subscribing to %s.%s." % (dt._name, variable))
                        sys.stdout.flush()
                        v = getattr(traci.constants, variable)
                        if dt._name == "edge":
                            name = "1si"
                        elif dt._name == "lane":
                            name = "1si_0"
                        elif dt._name in ("junction", "trafficlight"):
                            name = "A" if "CONSTRAINT" in variable else "0"
                        else:
                            name = dt._name + "_0"
                        param = None
                        if '"s"' in remainder:
                            param = {v: "3o_0"}
                        elif '"d"' in remainder:
                            param = {v: 0.}
                        elif '"i"' in remainder:
                            param = {v: 0}
                        elif '"b"' in remainder:
                            param = {v: ("b", 1)}
                        elif '"B"' in remainder:
                            param = {v: ("B", 0)}
                        elif '"tds"' in remainder:  # effort or traveltime for vehicle
                            param = {v: ("tds", 2, 0., "1si")}
                        elif '"tdddds"' in remainder:  # follow speed for vehicle
                            param = {v: ("tdddds", 5, 0., 0., 0., 0., "1si")}
                        elif '"tddds"' in remainder:  # secure gap for vehicle
                            param = {v: ("tddds", 4, 0., 0., 0., "1si")}
                        elif '"tdd"' in remainder:  # stop speed for vehicle
                            param = {v: ("tdd", 2, 0., 0.)}
                        elif '"tisb"' in remainder:  # stop parameter for vehicle
                            param = {v: ("tisb", 3, 0, "edge", False)}
                        elif '"tru"' in remainder:
                            param = {v: ("tru", 2, ("1si", 0., 0), traci.constants.REQUEST_DRIVINGDIST)}
                        elif '"tou"' in remainder:
                            param = {v: ("tou", 2, (400., 495.), traci.constants.REQUEST_DRIVINGDIST)}
                        try:
                            if dt._name == "simulation":
                                traci.simulation.subscribe([v], parameters=param)
                            else:
                                getattr(traci, dt._name).subscribe(name, [v], parameters=param)
                        except traci.TraCIException as ex:
                            print(ex)
        print("Expecting failure for %s.ADD subscription." % dt._name)
        sys.stdout.flush()
        try:
            if dt._name == "simulation":
                traci.simulation.subscribe([traci.constants.ADD])
            else:
                getattr(traci, dt._name).subscribe(name, [traci.constants.ADD])
        except traci.TraCIException as ex:
            print(ex)
    traci.simulationStep()
    for dt in pure_traci.DOMAINS:
        pprint(getattr(traci, dt._name).getAllSubscriptionResults())
finally:
    traci.close()
