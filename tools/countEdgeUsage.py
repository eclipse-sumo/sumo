#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    countEdgeUsage.py
# @author  Jakob Erdmann
# @date    2015-08-05

from __future__ import print_function
import sys
from optparse import OptionParser
from collections import defaultdict
from sumolib.output import parse_fast, parse
from sumolib.miscutils import Statistics, parseTime


def parse_args():
    DEFAULT_ELEMENTS = ['trip', 'route', 'walk']
    # when departure time must be known
    DEFAULT_ELEMENTS2 = ['vehicle']

    USAGE = "Usage: " + sys.argv[0] + " <routefile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--output-file", dest="outfile",
                         help="name of output file")
    optParser.add_option("--subpart",
                         help="Restrict counts to routes that contain the given consecutive edge sequence")
    optParser.add_option("--subpart-file", dest="subpart_file",
                         help="Restrict counts to routes that contain one of the consecutive edge sequences " +
                              "in the given input file (one sequence per line)")
    optParser.add_option("-i", "--intermediate", action="store_true", default=False,
                         help="count all edges of a route")
    optParser.add_option("--taz", action="store_true", default=False,
                         help="use fromTaz and toTaz instead of from and to")
    optParser.add_option("--elements",  default=','.join(DEFAULT_ELEMENTS),
                         help="include edges for the given elements in output")
    optParser.add_option("-b", "--begin", default=0, help="collect departures after begin time")
    optParser.add_option("-e", "--end", help="collect departures up to end time (default unlimited)")
    optParser.add_option("--period", help="create data intervals of the given period duration")
    options, args = optParser.parse_args()
    try:
        options.routefile, = args
    except Exception:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.routefile + ".departsAndArrivals.xml"

    options.subparts = []
    if options.subpart is not None:
        options.subparts.append(options.subpart.split(','))
    if options.subpart_file is not None:
        for line in open(options.subpart_file):
            options.subparts.append(line.strip().split(','))

    options.elements = options.elements.split(',')

    options.begin = parseTime(options.begin)
    if options.end is not None:
        options.end = parseTime(options.end)
    if options.period is not None:
        options.period = parseTime(options.period)

    options.elements2 = []
    if options.begin != 0 or options.end is not None or options.period:
        if options.elements == DEFAULT_ELEMENTS:
            options.elements2 = DEFAULT_ELEMENTS2
        else:
            for elem in options.elements:
                if elem not in DEFAULT_ELEMENTS2:
                    sys.stderr.write("Element '%s' does not supply departure time. Use one of %s instead.\n" %
                                     (elem, DEFAULT_ELEMENTS2))
                else:
                    options.elements2.append(elem)
            options.elements = []

    if options.end is None:
        options.end = 1e100

    return options


def hasSubpart(edges, subparts):
    if not subparts:
        return True
    for subpart in subparts:
        for i in range(len(edges)):
            if edges[i:i + len(subpart)] == subpart:
                return True
    return False


def writeInterval(outf, options, departCounts, arrivalCounts, intermediateCounts, begin=0, end="10000", prefix=""):
    departStats = Statistics(prefix + "departEdges")
    arrivalStats = Statistics(prefix + "arrivalEdges")
    intermediateStats = Statistics(prefix + "intermediateEdges")
    for e in sorted(departCounts.keys()):
        departStats.add(departCounts[e], e)
    for e in sorted(arrivalCounts.keys()):
        arrivalStats.add(arrivalCounts[e], e)
    print(departStats)
    print(arrivalStats)
    if options.intermediate:
        for e in sorted(intermediateCounts.keys()):
            intermediateStats.add(intermediateCounts[e], e)
        print(intermediateStats)

    outf.write('   <interval begin="%s" end="%s" id="routeStats">\n' % (begin, end))
    allEdges = set(departCounts.keys())
    allEdges.update(arrivalCounts.keys())
    if options.intermediate:
        allEdges.update(intermediateCounts.keys())
    for e in sorted(allEdges):
        intermediate = ' intermediate="%s"' % intermediateCounts[e] if options.intermediate else ''
        outf.write('      <edge id="%s" departed="%s" arrived="%s" delta="%s"%s/>\n' %
                   (e, departCounts[e], arrivalCounts[e], arrivalCounts[e] - departCounts[e], intermediate))
    outf.write("   </interval>\n")
    departCounts.clear()
    arrivalCounts.clear()
    intermediateCounts.clear()


def parseSimple(outf, options):
    """parse elements without checking time"""
    departCounts = defaultdict(lambda: 0)
    arrivalCounts = defaultdict(lambda: 0)
    intermediateCounts = defaultdict(lambda: 0)

    for element in options.elements:
        for route in parse_fast(options.routefile, element, ['edges']):
            edges = route.edges.split()
            if not hasSubpart(edges, options.subparts):
                continue
            departCounts[edges[0]] += 1
            arrivalCounts[edges[-1]] += 1
            for e in edges:
                intermediateCounts[e] += 1

    # warn about potentially missing edges
    fromAttr, toAttr = ('fromTaz', 'toTaz') if options.taz else ('from', 'to')
    if 'trip' in options.elements:
        for trip in parse_fast(options.routefile, 'trip', ['id', fromAttr, toAttr]):
            if options.subparts:
                sys.stderr.write("Warning: Ignoring trips when using --subpart\n")
                break
            departCounts[trip[1]] += 1
            arrivalCounts[trip[2]] += 1
    if 'walk' in options.elements:
        for walk in parse_fast(options.routefile, 'walk', ['from', 'to']):
            if options.subparts:
                sys.stderr.write("Warning: Ignoring trips when using --subpart\n")
                break
            departCounts[walk.attr_from] += 1
            arrivalCounts[walk.to] += 1

    writeInterval(outf, options, departCounts, arrivalCounts, intermediateCounts)


def parseTimed(outf, options):
    departCounts = defaultdict(lambda: 0)
    arrivalCounts = defaultdict(lambda: 0)
    intermediateCounts = defaultdict(lambda: 0)
    lastDepart = 0
    period = options.period if options.period else options.end
    begin = options.begin
    periodEnd = options.period if options.period else options.end

    for elem in parse(options.routefile, options.elements2):
        depart = elem.depart
        if depart != "triggered":
            depart = parseTime(depart)
            lastDepart = depart
            if depart < lastDepart:
                sys.stderr.write("Unsorted departure %s for %s '%s'" % (
                    depart, elem.tag, elem.id))
                lastDepart = depart
            if depart < begin:
                continue
            if depart >= periodEnd or depart >= options.end:
                description = "%s-%s " % (begin, periodEnd)
                writeInterval(outf, options, departCounts, arrivalCounts,
                              intermediateCounts, begin, periodEnd, description)
                periodEnd += period
                begin += period
            if depart >= options.end:
                break
        if elem.route:
            edges = elem.route[0].edges.split()
            if not hasSubpart(edges, options.subparts):
                continue
            departCounts[edges[0]] += 1
            arrivalCounts[edges[-1]] += 1
            for e in edges:
                intermediateCounts[e] += 1

    description = "%s-%s " % (begin, periodEnd)
    if len(departCounts) > 0:
        writeInterval(outf, options, departCounts, arrivalCounts, intermediateCounts, begin, lastDepart, description)


def main():
    options = parse_args()
    outf = open(options.outfile, 'w')
    outf.write("<edgedata>\n")

    if options.elements2:
        parseTimed(outf, options)
    else:
        parseSimple(outf, options)

    outf.write("</edgedata>\n")
    outf.close()


if __name__ == "__main__":
    main()
