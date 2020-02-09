#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    routeSampler.py
# @author  Jakob Erdmann
# @date    2020-02-07

"""
Samples routes from a given set to fullfill specified counting data (edge counts or turn counts)
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import random
from argparse import ArgumentParser
from collections import defaultdict
import subprocess

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    parser = ArgumentParser(description="Sample routes to match counts")
    parser.add_argument("-t", "--turn-file", dest="turnFile",
            help="Input turn-count file")
    parser.add_argument("-d", "--edgedata-file", dest="edgeDataFile",
            help="Input edgeData file file (for counts)")
    parser.add_argument("--edgedata-attribute", dest="edgeDataAttr", default="entered",
            help="Read edgeData counts from the given attribute")
    parser.add_argument("--turn-attribute", dest="turnAttr", default="probability",
            help="Read turning counts from the given attribute")
    parser.add_argument("-r", "--route-file", dest="routeFile",
            help="Input route file file")
    parser.add_argument("-o", "--output-file", dest="out", default="out.rou.xml",
            help="Output route file")
    parser.add_argument("--prefix", dest="prefix", default="",
            help="prefix for the vehicle ids")
    parser.add_argument("-a", "--attributes", dest="vehattrs", default="", 
            help="additional vehicle attributes")
    parser.add_argument("-s", "--seed", type=int, default=42,
            help="random seed")

    options = parser.parse_args(args=args)
    if (options.routeFile is None or
            (options.turnFile is None and options.edgeDataFile is None)):
        parser.print_help()
        sys.exit()
    options.turnFile = options.turnFile.split(',') if options.turnFile is not None else []
    options.edgeDataFile = options.edgeDataFile.split(',') if options.edgeDataFile is not None else []
    return options

class CountData:
    def __init__(self, count, edgeTuple, allRoutes):
        self.count = count
        self.edgeTuple = edgeTuple
        self.routeSet = set()
        for routeIndex, edges in enumerate(allRoutes):
            if self.routePasses(edges):
                self.routeSet.add(routeIndex)
        if self.count > 0 and not self.routeSet:
            print("Warning: no routes pass edge '%s' (count %s)" %
                    (' '.join(self.edgeTuple), self.count), file=sys.stderr)

    def routePasses(self, edges):
        try:
            i = edges.index(self.edgeTuple[0])
            if self.edgeTuple != tuple(edges[i:i + len(self.edgeTuple)]):
                return False
        except ValueError:
            # first edge not in route
            return False
        return True

def parseTurnCounts(fnames, allRoutes, attr):
    result = []
    for fname in fnames:
        for interval in sumolib.xml.parse(fname, 'interval'):
            for fromEdge in interval.fromEdge:
                for toEdge in fromEdge.toEdge:
                    result.append(CountData(int(getattr(toEdge, attr)),
                        (fromEdge.id, toEdge.id), allRoutes))
    return result

def parseEdgeCounts(fnames, allRoutes, attr):
    result = []
    for fname in fnames:
        for interval in sumolib.xml.parse(fname, 'interval'):
            for edge in interval.edge:
                result.append(CountData(int(getattr(edge, attr)),
                    (edge.id,), allRoutes))
    return result

def parseTimeRange(fnames):
    begin = 1e20
    end = 0
    for fname in fnames:
        for interval in sumolib.xml.parse(fname, 'interval'):
            begin = min(begin, float(interval.begin))
            end = max(end, float(interval.end))
    return begin, end


def hasCapacity(dataIndices, countData):
    for i in dataIndices:
        if countData[i].count == 0:
            return False
    return True

def updateOpenRoutes(openRoutes, routeUsage, countData):
    return filter(lambda r : hasCapacity(routeUsage[r], countData), openRoutes) 

def updateOpenCounts(openCounts, countData, openRoutes):
    return filter(lambda i : countData[i].routeSet.intersection(openRoutes), openCounts)

def main(options):
    if options.seed:
        random.seed(options.seed)

    # store which routes are passing each counting location (using route index)
    routes = [r.edges.split() for r in sumolib.xml.parse_fast(options.routeFile, 'route', ['edges'])]
    countData = (parseTurnCounts(options.turnFile, routes, options.turnAttr) 
        + parseEdgeCounts(options.edgeDataFile, routes, options.edgeDataAttr))

    # store which counting locations are used by each route (using countData index)
    routeUsage = [set() for r in routes]
    for i, cd in enumerate(countData):
        for routeIndex in cd.routeSet:
            routeUsage[routeIndex].add(i)

    # pick a random couting location and select a new route that passes it until
    # all counts are satisfied or no routes can be used anymore
    openRoutes = set(range(0, len(routes)))
    openCounts = set(range(0, len(countData)))
    openRoutes = updateOpenRoutes(openRoutes, routeUsage, countData)
    openCounts = updateOpenCounts(openCounts, countData, openRoutes)

    usedRoutes = []
    totalCount = 0
    while openCounts:   
        cd = countData[random.sample(openCounts, 1)[0]]
        routeIndex = random.sample(cd.routeSet.intersection(openRoutes), 1)[0]
        usedRoutes.append(routeIndex)
        for dataIndex in routeUsage[routeIndex]:
            countData[dataIndex].count -= 1
            totalCount += 1
        openRoutes = updateOpenRoutes(openRoutes, routeUsage, countData)
        openCounts = updateOpenCounts(openCounts, countData, openRoutes)

    begin, end = parseTimeRange(options.turnFile + options.edgeDataFile)
    with open(options.out, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes")  # noqa
        period = (end - begin) / len(usedRoutes)
        depart = begin
        for i, routeIndex in enumerate(usedRoutes):
            outf.write('    <vehicle id="%s%s" depart="%s"%s>\n' % (
                options.prefix, i, depart, options.vehattrs))
            outf.write('        <route edges="%s"/>\n' % ' '.join(routes[routeIndex]))
            outf.write('    </vehicle>\n')
            depart += period
        outf.write('</routes>\n')

    unfilledLocations = 0
    missingCount = 0
    for cd in countData:
        if cd.count > 0:
            unfilledLocations += 1
            missingCount += cd.count

    print("Wrote %s routes to meet total count %s at %s locations" % (
        len(usedRoutes), totalCount, len(countData)))

    if unfilledLocations > 0:
        print("Warning: Count deficit of %s for %s locations" %
                    (missingCount, unfilledLocations))

if __name__ == "__main__":
    main(get_options())
