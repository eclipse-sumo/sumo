#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    vehrouteCountValidation.py
# @author  Jakob Erdmann
# @date    2022-10-20

"""
Compare counting data (edge counts or turn counts)
to counts obtained from a simulation (--vehroute-output, with exit-times).
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    THIS_DIR = os.path.abspath(os.path.dirname(__file__))
    sys.path.append(os.path.join(THIS_DIR, '..'))

import sumolib  # noqa
from sumolib.miscutils import parseTime  # noqa
from routeSampler import getIntervals, parseCounts  # noqa


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    parser.add_argument("-r", "--route-files", dest="routeFiles",
                        help="Input route file (vehroute-output)")
    parser.add_argument("-t", "--turn-files", dest="turnFiles",
                        help="Input turn-count file")
    parser.add_argument("-d", "--edgedata-files", dest="edgeDataFiles",
                        help="Input edgeData file (for counts)")
    parser.add_argument("-O", "--od-files", dest="odFiles",
                        help="Input edgeRelation file for origin-destination counts")
    parser.add_argument("--edgedata-attribute", dest="edgeDataAttr", default="entered",
                        help="Read edgeData counts from the given attribute")
    parser.add_argument("--arrival-attribute", dest="arrivalAttr",
                        help="Read arrival counts from the given edgeData file attribute")
    parser.add_argument("--depart-attribute", dest="departAttr",
                        help="Read departure counts from the given edgeData file attribute")
    parser.add_argument("--turn-attribute", dest="turnAttr", default="count",
                        help="Read turning counts from the given attribute")
    parser.add_argument("--turn-ratio-attribute", dest="turnRatioAttr",
                        help="Read turning ratios from the given attribute")
    parser.add_argument("--turn-ratio-total", dest="turnRatioTotal", type=float, default=1,
                        help="Set value for normalizing turning ratios (default 1)")
    parser.add_argument("--turn-max-gap", type=int, dest="turnMaxGap", default=0,
                        help="Allow at most a gap of INT edges between from-edge and to-edge")
    parser.add_argument("-m", "--mismatch-files", dest="mismatchFiles",
                        help="Input mismatch file (as computed by routeSampler.py) to compensate sampling errors")
    parser.add_argument("--prefix", dest="prefix", default="",
                        help="prefix for the vehicle ids")
    parser.add_argument("--type", dest="type",
                        help="vehiclet type to filter by")
    parser.add_argument("--mismatch-output", dest="mismatchOut", default="mismatch.xml",
                        help="write count-data with overflow/underflow information to FILE")
    parser.add_argument("--geh-ok", dest="gehOk", type=float, default=5,
                        help="threshold for acceptable GEH values")
    parser.add_argument("--pedestrians", action="store_true", default=False,
                        help="compare person walks instead of vehicle routes")
    parser.add_argument("-b", "--begin", help="custom begin time (seconds or H:M:S)")
    parser.add_argument("-e", "--end", help="custom end time (seconds or H:M:S)")
    parser.add_argument("-i", "--interval", help="custom aggregation interval (seconds or H:M:S)")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="tell me what you are doing")
    parser.add_argument("-V", "--verbose.histograms", dest="verboseHistogram", action="store_true", default=False,
                        help="print histograms of edge numbers and detector passing count")

    options = parser.parse_args(args=args)
    if (options.routeFiles is None or
            (options.turnFiles is None and options.edgeDataFiles is None and options.odFiles is None)):
        parser.print_help()
        sys.exit()

    options.routeFiles = options.routeFiles.split(',')
    options.turnFiles = options.turnFiles.split(',') if options.turnFiles is not None else []
    options.edgeDataFiles = options.edgeDataFiles.split(',') if options.edgeDataFiles is not None else []
    options.odFiles = options.odFiles.split(',') if options.odFiles is not None else []

    return options


class Routes:
    """dummy class to allow using the same methods as routeSampler.py"""
    def __init__(self):
        self.unique = []


def main(options):
    intervals = getIntervals(options)

    if len(intervals) == 0:
        print("Error: no intervals loaded", file=sys.stderr)
        sys.exit()

    mismatchf = open(options.mismatchOut, 'w')
    sumolib.writeXMLHeader(mismatchf, "$Id$", options=options)  # noqa
    mismatchf.write('<data>\n')

    underflowSummary = sumolib.miscutils.Statistics("avg interval underflow")
    overflowSummary = sumolib.miscutils.Statistics("avg interval overflow")
    gehSummary = sumolib.miscutils.Statistics("avg interval GEH%")
    inputCountSummary = sumolib.miscutils.Statistics("avg interval input count")
    usedRoutesSummary = sumolib.miscutils.Statistics("avg interval loaded vehs")

    for begin, end in intervals:
        intervalPrefix = "" if len(intervals) == 1 else "%s_" % int(begin)
        uFlow, oFlow, gehOK, inputCount, usedRoutes = checkInterval(options, begin, end, intervalPrefix, mismatchf)
        underflowSummary.add(uFlow, begin)
        overflowSummary.add(oFlow, begin)
        gehSummary.add(gehOK, begin)
        inputCountSummary.add(inputCount, begin)
        usedRoutesSummary.add(usedRoutes, begin)

    mismatchf.write('</data>\n')
    mismatchf.close()

    if len(intervals) > 1:
        print(inputCountSummary)
        print(usedRoutesSummary)
        print(underflowSummary)
        print(overflowSummary)
        print(gehSummary)


def checkInterval(options, begin, end, intervalPrefix, mismatchf):
    routes = Routes()
    countData = parseCounts(options, routes, begin, end)

    edgeCount = sumolib.miscutils.Statistics("route edge count", histogram=True)
    detectorCount = sumolib.miscutils.Statistics("route detector count", histogram=True)
    usedRoutes = []

    for routeFile in options.routeFiles:
        for vehicle in sumolib.xml.parse(routeFile, "vehicle"):
            if options.type and vehicle.type != options.type:
                continue
            depart = parseTime(vehicle.depart)
            if depart >= end:
                continue
            route = vehicle.route[0]
            edges = tuple(route.edges.split())

            exitTimes = []
            if route.exitTimes:
                exitTimes = tuple(map(parseTime, route.exitTimes.split()))
            else:
                exitTimes = [depart] * len(edges)

            numPassedDets = 0
            for cd in countData:
                i = cd.routePasses(edges)
                if i is not None:
                    numPassedDets += 1
                    et = exitTimes[i]
                    if et < begin or et >= end:
                        continue
                    cd.use()
            usedRoutes.append(edges)
            edgeCount.add(len(edges), vehicle.id)
            detectorCount.add(numPassedDets, vehicle.id)

    underflow = sumolib.miscutils.Statistics("underflow locations")
    overflow = sumolib.miscutils.Statistics("overflow locations")
    gehStats = sumolib.miscutils.Statistics("GEH")
    numGehOK = 0.0
    hourFraction = (end - begin) / 3600.0
    totalCount = 0
    totalOrigCount = 0
    for cd in countData:
        if cd.isRatio:
            continue
        localCount = cd.origCount - cd.count
        totalCount += localCount
        totalOrigCount += cd.origCount
        if cd.count > 0:
            underflow.add(cd.count, cd.edgeTuple)
        elif cd.count < 0:
            overflow.add(cd.count, cd.edgeTuple)
        origHourly = cd.origCount / hourFraction
        localHourly = localCount / hourFraction
        geh = sumolib.miscutils.geh(origHourly, localHourly)
        if geh < options.gehOk:
            numGehOK += 1
        gehStats.add(geh, "[%s] %s %s" % (
            ' '.join(cd.edgeTuple), int(origHourly), int(localHourly)))

    outputIntervalPrefix = "" if intervalPrefix == "" else "%s: " % int(begin)
    countPercentage = "%.2f%%" % (100 * totalCount / float(totalOrigCount)) if totalOrigCount else "-"
    gehOKNum = 100 * numGehOK / float(len(countData)) if countData else 100
    gehOK = "%.2f%%" % gehOKNum if countData else "-"
    print("%sRead %s routes (%s distinct) achieving total count %s (%s) at %s locations. GEH<%s for %s" % (
        outputIntervalPrefix,
        len(usedRoutes), len(set(usedRoutes)),
        totalCount, countPercentage, len(countData),
        options.gehOk, gehOK))

    if options.verboseHistogram:
        print("result %s" % edgeCount)
        print("result %s" % detectorCount)
        print(gehStats)

    if underflow.count() > 0:
        print("Warning: %s (total %s)" % (underflow, sum(underflow.values)))
    if overflow.count() > 0:
        print("Warning: %s (total %s)" % (overflow, sum(overflow.values)))
    sys.stdout.flush()  # needed for multiprocessing

    if mismatchf:
        mismatchf.write('    <interval id="deficit" begin="%s" end="%s">\n' % (begin, end))
        for cd in countData:
            if len(cd.edgeTuple) == 1:
                mismatchf.write('        <edge id="%s" measuredCount="%s" deficit="%s"/>\n' % (
                    cd.edgeTuple[0], cd.origCount, cd.count))
            elif len(cd.edgeTuple) == 2:
                mismatchf.write('        <edgeRelation from="%s" to="%s" measuredCount="%s" deficit="%s"/>\n' % (
                    cd.edgeTuple[0], cd.edgeTuple[1], cd.origCount, cd.count))
            else:
                print("Warning: output for edge relations with more than 2 edges not supported (%s)" % cd.edgeTuple,
                      file=sys.stderr)
        mismatchf.write('    </interval>\n')

    return sum(underflow.values), sum(overflow.values), gehOKNum, totalOrigCount, len(usedRoutes)


if __name__ == "__main__":
    main(get_options())
