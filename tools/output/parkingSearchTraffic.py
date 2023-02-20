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

# @file    parkingSearchTraffic.py
# @author  Michael Behrisch
# @date    2021-11-05

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa


def parse_args():
    optParser = ArgumentParser()
    optParser.add_argument("net", help="net file")
    optParser.add_argument("routes", help="route file")
    return optParser.parse_args()


def main(net, routes):
    net = sumolib.net.readNet(net)
    dist = sumolib.miscutils.Statistics("Distance")
    time = sumolib.miscutils.Statistics("Time")
    walk_dist = sumolib.miscutils.Statistics("Walking Distance")
    for vehicle in sumolib.xml.parse(routes, 'vehicle'):
        if not vehicle.stop:
            print("Warning! Vehicle '%s' did not arrive." % vehicle.id)
            continue
        if vehicle.routeDistribution and vehicle.stop:
            replace_index = None
            for r in vehicle.routeDistribution[0].route:
                if replace_index is None and r.replacedOnEdge:
                    replace_index = len(r.edges.split())
                    replace_time = r.replacedAtTime
            extra_route = r.edges.split()[replace_index:]
            length = sum([net.getEdge(e).getLength() for e in extra_route])
            dist.add(length, vehicle.id)
            time.add(sumolib.miscutils.parseTime(vehicle.stop[0].started) -
                     sumolib.miscutils.parseTime(replace_time), vehicle.id)
            if extra_route:
                walk, _ = net.getShortestPath(net.getEdge(extra_route[-1]), net.getEdge(extra_route[0]),
                                              ignoreDirection=True)
                walk_length = sum([e.getLength() for e in walk])
            else:
                walk_length = 0
            walk_dist.add(walk_length, vehicle.id)
        else:
            dist.add(0, vehicle.id)
            time.add(0, vehicle.id)
            walk_dist.add(0, vehicle.id)
    print(dist)
    print(time)
    print(walk_dist)


if __name__ == "__main__":
    options = parse_args()
    main(options.net, options.routes)
