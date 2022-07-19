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

# @file    countEdgeUsage.py
# @author  Jakob Erdmann
# @date    2015-08-05

from __future__ import print_function
import sys
from optparse import OptionParser
from collections import defaultdict
import sumolib
from sumolib.output import parse_fast, parse
from sumolib.miscutils import Statistics, parseTime, getFlowNumber

END_UNLIMITED = 1e100


def parse_args():
    DEFAULT_ELEMENTS = ['trip', 'route', 'walk']
    # when departure time must be known
    DEFAULT_ELEMENTS2 = ['vehicle', 'trip', 'flow']

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
    optParser.add_option("-m", "--min-count", default=0, type=int, help="include only values above the minimum")
    optParser.add_option("-n", "--net-file", help="parse net for geo locations of the edges")
    optParser.add_option("-p", "--poi-file", help="write geo POIs")
    options, args = optParser.parse_args()
    try:
        options.routefile, = args
    except Exception:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.routefile + ".departsAndArrivals.xml"
    if options.net_file and not options.poi_file:
        options.poi_file = options.net_file + "_count.poi.xml"

    options.subparts = []
    if options.subpart is not None:
        options.subparts.append(options.subpart.split(','))
    if options.subpart_file is not None:
        with open(options.subpart_file) as subparts:
            for line in subparts:
                options.subparts.append(line.strip().split(','))
    if options.taz:
        for subpart in options.subparts:
            if len(subpart) > 2:
                sys.stderr.write("At most two elements can be in a subpart when using --taz (found %s)\n" % subpart)
                sys.exit(1)

    options.elements = options.elements.split(',')

    options.begin = parseTime(options.begin)
    if options.end is not None:
        options.end = parseTime(options.end)
    if options.period is not None:
        options.period = parseTime(options.period)

    options.elements2 = []
    if options.begin != 0 or options.end is not None or options.period or options.taz or 'flow' in options.elements:
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
        options.end = END_UNLIMITED

    return options


def hasSubpart(edges, subparts):
    if not subparts:
        return True
    for subpart in subparts:
        for i in range(len(edges)):
            if edges[i:i + len(subpart)] == subpart:
                return True
    return False


def getEdges(elem, taz):
    edges = []
    src = None
    dst = None
    if elem.edges:
        edges = elem.edges.split()
    if elem.route:
        edges = elem.route[0].edges.split()
    if edges:
        src = edges[0]
        dst = edges[-1]
    try:
        if taz:
            src = elem.fromTaz
            dst = elem.toTaz
        elif not edges:
            src = elem.attr_from
            dst = elem.to
    except AttributeError:
        pass
    return src, dst, edges


def writeInterval(outf, options, departCounts, arrivalCounts, intermediateCounts, begin=0, end="1000000", prefix=""):
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

    outf.write('    <interval begin="%s" end="%s" id="routeStats">\n' % (begin, end))
    allEdges = set(departCounts.keys())
    allEdges.update(arrivalCounts.keys())
    if options.intermediate:
        allEdges.update(intermediateCounts.keys())
    for e in sorted(allEdges):
        intermediate = ' intermediate="%s"' % intermediateCounts[e] if options.intermediate else ''
        if (departCounts[e] > options.min_count or arrivalCounts[e] > options.min_count or
                intermediateCounts[e] > 0):
            outf.write('        <edge id="%s" departed="%s" arrived="%s" delta="%s"%s/>\n' %
                       (e, departCounts[e], arrivalCounts[e], arrivalCounts[e] - departCounts[e], intermediate))
    outf.write("    </interval>\n")
    departCounts.clear()
    arrivalCounts.clear()
    intermediateCounts.clear()


def parseSimple(outf, options):
    """parse elements without checking time (uses fast parser)"""
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
            if options.intermediate:
                for e in edges:
                    intermediateCounts[e] += 1

    # warn about potentially missing edges
    fromAttr, toAttr = ('fromTaz', 'toTaz') if options.taz else ('from', 'to')
    if 'trip' in options.elements:
        for trip in parse_fast(options.routefile, 'trip', ['id', fromAttr, toAttr]):
            if not hasSubpart([trip[1], trip[2]], options.subparts):
                continue
            departCounts[trip[1]] += 1
            arrivalCounts[trip[2]] += 1
    if 'walk' in options.elements:
        for walk in parse_fast(options.routefile, 'walk', ['from', 'to']):
            if not hasSubpart([walk[1], walk[2]], options.subparts):
                continue
            departCounts[walk.attr_from] += 1
            arrivalCounts[walk.to] += 1

    if options.net_file:
        net = sumolib.net.readNet(options.net_file)
        with open(options.poi_file, "w") as pois:
            sumolib.xml.writeHeader(pois, root="additional")
            allEdges = set(departCounts.keys())
            allEdges.update(arrivalCounts.keys())
            for e in sorted(allEdges):
                if departCounts[e] > options.min_count or arrivalCounts[e] > options.min_count:
                    lon, lat = net.convertXY2LonLat(*net.getEdge(e).getShape()[0])
                    pois.write('    <poi id="%s" lon="%.6f" lat="%.6f">\n' % (e, lon, lat))
                    pois.write('        <param key="departed" value="%s"/>\n' % departCounts[e])
                    pois.write('        <param key="arrived" value="%s"/>\n    </poi>\n' % arrivalCounts[e])
            pois.write("</additional>\n")
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
        depart = elem.depart if elem.depart is not None else elem.begin
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
        number = getFlowNumber(elem) if elem.name == 'flow' else 1
        src, dst, edges = getEdges(elem, options.taz)
        filterBy = [src, dst] if options.taz or not edges else edges
        if not hasSubpart(filterBy, options.subparts):
            continue
        departCounts[src] += number
        arrivalCounts[dst] += number
        if options.intermediate:
            for e in edges:
                intermediateCounts[e] += number

    description = "%s-%s " % (begin, periodEnd) if periodEnd != END_UNLIMITED else ""
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
