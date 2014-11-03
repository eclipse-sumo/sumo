#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    vehroute2amitranOD.py
@author  Michael Behrisch
@date    2014-04-08
@version $Id$

Convert a vehroute-output file into amitran route and od files

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os,sys
from collections import defaultdict
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
import sumolib

def convert(vehRoutes, routeOut, odOut, interval):
    routeDict = {}
    actorConfig = defaultdict(list) # map type -> list of time slices
    with open(routeOut, 'w') as routes:
        routes.write("<routes>\n")
        for v in sumolib.output.parse(vehRoutes, 'vehicle'):
            depart = float(v.depart)
            travelTime = float(v.arrival) - depart
            if hasattr(v, "routeDistribution"):
                edges = v.routeDistribution[0].route[-1].edges
            else:
                edges = v.route[0].edges
            ac = getattr(v, "type", "DEFAULT_VEHTYPE")
            if edges not in routeDict:
                idx = len(routeDict)
                routeDict[edges] = idx
                routes.write('    <route id="%s">\n' % idx)
                for e in edges.split():
                    routes.write('        <link id="%s"/>\n' % e)
                routes.write('    </route>\n')
            else:
                idx = routeDict[edges]
            listPos = int(depart / interval)
            while len(actorConfig[ac]) <= listPos:
                actorConfig[ac].append(defaultdict(dict))
            od = actorConfig[ac][listPos] # map (origin, dest) -> map route -> (amount, travel time sum)
            key = (v.fromTaz, v.toTaz)
            if idx in od[key]:
                oldValue = od[key][idx]
                value = (oldValue[0] + 1, oldValue[1] + travelTime)
            else:
                value = (1, travelTime)
            od[key][idx] = value
        routes.write("</routes>\n")
    with open(odOut, 'w') as od:
        od.write("<demand>\n")
        for ac, odList in actorConfig.iteritems():
            od.write('    <actorConfig id="%s">\n' % ac)
            for idx, odMap in enumerate(odList):
                if odMap:
                    od.write('        <timeSlice startTime="%s" duration="%s">\n' % (idx * interval * 1000, interval * 1000))
                    for (orig, dest), routeMap in odMap.iteritems():
                        total = 0
                        for amount, _ in routeMap.itervalues():
                            total += amount
                        od.write('            <odPair origin="%s" destination="%s" amount="%s">\n' % (orig, dest, total))
                        for idx, (amount, ttSum) in routeMap.iteritems():
                            od.write('                <routeCost routeId="%s" amount="%s" averageTraveltime="%s"/>\n' % (idx, amount, int(1000. * ttSum / amount)))
                            total += amount
                        od.write('            </odPair>\n')
                    od.write('        </timeSlice>\n')
            od.write('    <actorConfig/>\n')
        od.write("</demand>\n")

if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option("-r", "--routes", default='routes.xml',
                         help="name of the amitran route file output [default: %default]")
    optParser.add_option("-o", "--od-file", default='od.xml',
                         help="name of the amitran O/D file output [default: %default]")
    optParser.add_option("-i", "--interval", default=3600, type=int,
                         help="aggregation interval in seconds [default: %default]")
    (options, args) = optParser.parse_args()
    convert(args[0], options.routes, options.od_file, options.interval)
 