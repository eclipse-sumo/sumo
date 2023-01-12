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

# @file    fcdReplay.py
# @author  Jakob Erdmann
# @date    2023-01-11

"""
Replay an fcd-file as moving POIs on top of a simulation (or emtpy network)
"""

from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
import traci  # noqa


def main():
    parser = sumolib.options.ArgumentParser()
    parser.add_argument("-k", "--sumo-config", default="sumo.sumocfg", help="sumo config file")
    parser.add_argument("-f", "--fcd-files", dest="fcdFiles", help="the fcd files to replay")
    parser.add_argument("--geo", action="store_true", default=False, help="use fcd data in lon,lat format")
    parser.add_argument("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")
    options, args = parser.parse_known_args()

    sumoBinary = sumolib.checkBinary("sumo-gui")
    traci.start([sumoBinary, "-c", options.sumo_config] + args)
    t = traci.simulation.getTime()
    deltaT = traci.simulation.getDeltaT()

    fcdData = defaultdict(list)  # time -> objects
    lastTime = {}  # objectID -> last known time
    for fname in options.fcdFiles.split(','):
        if options.verbose:
            print("Loading fcd data from '%s'" % fname)
        for ts in sumolib.xml.parse(fname, 'timestep'):
            time = sumolib.miscutils.parseTime(ts.time)
            if time < t:
                continue
            for obj in ts.getChildList():
                obj.x = float(obj.x)
                obj.y = float(obj.y)
                if options.geo:
                    obj.x, obj.y = traci.simulation.convertGeo(obj.x, obj.y, True)
                fcdData[time].append(obj)
                lastTime[obj.id] = time

    removeAtTime = defaultdict(list)  # time -> objects to remove
    for oID, t in lastTime.items():
        removeAtTime[t + deltaT].append(oID)

    end = max(max(lastTime.values()), traci.simulation.getEndTime())
    created = set()

    while t <= end:
        for obj in fcdData.get(t, []):
            if obj.id in created:
                traci.poi.setPosition(obj.id, obj.x, obj.y)
            else:
                traci.poi.add(obj.id, obj.x, obj.y, (255, 0, 0, 255))
                created.add(obj.id)
            for a, v in obj.getAttributes():
                if a == 'x' or a == 'y' or a == 'id':
                    continue
                traci.poi.setParameter(obj.id, a, v)
        for objID in removeAtTime.get(t, []):
            traci.poi.remove(objID)

        traci.simulationStep()
        t = traci.simulation.getTime()

    traci.close()


if __name__ == "__main__":
    main()
