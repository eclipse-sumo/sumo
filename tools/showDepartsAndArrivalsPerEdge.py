#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    showDepartsAndArrivalsPerEdge.py
# @author  Jakob Erdmann
# @date    2015-08-05
# @version $Id$

from __future__ import print_function
import sys
from optparse import OptionParser
from collections import defaultdict
from sumolib.output import parse_fast
from sumolib.miscutils import Statistics


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <routefile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--output-file",
                         dest="outfile", help="name of output file")
    optParser.add_option(
        "--subpart", help="Restrict counts to routes that contain the given consecutive edge sequence")
    options, args = optParser.parse_args()
    try:
        options.routefile, = args
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.routefile + ".departsAndArrivals.xml"
    if options.subpart is not None:
        options.subpart = options.subpart.split(',')
    return options


def hasSubpart(edges, subpart):
    for i in range(len(edges)):
        if edges[i:i + len(subpart)] == subpart:
            return True
    return False


def main():
    options = parse_args()
    departCounts = defaultdict(lambda: 0)
    arrivalCounts = defaultdict(lambda: 0)

    for route in parse_fast(options.routefile, 'route', ['edges']):
        edges = route.edges.split()
        if options.subpart is not None and not hasSubpart(edges, options.subpart):
            continue
        departCounts[edges[0]] += 1
        arrivalCounts[edges[-1]] += 1
    for walk in parse_fast(options.routefile, 'walk', ['edges']):
        edges = walk.edges.split()
        if options.subpart is not None and not hasSubpart(edges, options.subpart):
            continue
        departCounts[edges[0]] += 1
        arrivalCounts[edges[-1]] += 1

    # warn about potentially missing edges
    for trip in parse_fast(options.routefile, 'trip', ['id', 'fromTaz', 'toTaz']):
        if options.subpart is not None:
            sys.stderr.print("Warning: Ignoring trips when using --subpart")
            break
        departCounts[trip.fromTaz] += 1
        arrivalCounts[trip.toTaz] += 1
    for walk in parse_fast(options.routefile, 'walk', ['from', 'to']):
        if options.subpart is not None:
            sys.stderr.print("Warning: Ignoring trips when using --subpart")
            break
        departCounts[walk.attr_from] += 1
        arrivalCounts[walk.to] += 1

    departStats = Statistics("departEdges")
    arrivalStats = Statistics("arrivalEdges")
    for e in sorted(departCounts.keys()):
        departStats.add(departCounts[e], e)
    for e in sorted(arrivalCounts.keys()):
        arrivalStats.add(arrivalCounts[e], e)
    print(departStats)
    print(arrivalStats)

    with open(options.outfile, 'w') as outf:
        outf.write("<edgedata>\n")
        outf.write('   <interval begin="0" end="10000" id="routeStats">\n')
        allEdges = set(departCounts.keys())
        allEdges.update(arrivalCounts.keys())
        for e in sorted(allEdges):
            outf.write('      <edge id="%s" departed="%s" arrived="%s" delta="%s"/>\n' %
                       (e, departCounts[e], arrivalCounts[e], arrivalCounts[e] - departCounts[e]))
        outf.write("   </interval>\n")
        outf.write("</edgedata>\n")


if __name__ == "__main__":
    main()
