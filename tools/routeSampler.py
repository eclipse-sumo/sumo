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
from collections import defaultdict
from math import ceil
# multiprocessing imports
import multiprocessing
import numpy as np

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime, humanReadableTime  # noqa

PRESERVE_INPUT_COUNT = 'input'


def _run_func(args):
    func, interval, kwargs, num = args
    kwargs["cpuIndex"] = num
    return num, func(interval=interval, **kwargs)


def multi_process(cpu_num, interval_list, func, outf, mismatchf, **kwargs):
    cpu_count = min(cpu_num, multiprocessing.cpu_count()-1, len(interval_list))
    interval_split = np.array_split(interval_list, cpu_count)
    # pool = multiprocessing.Pool(processes=cpu_count)
    with multiprocessing.get_context("spawn").Pool() as pool:
        results = pool.map(_run_func, [(func, interval, kwargs, i) for i, interval in enumerate(interval_split)])
        # pool.close()
        results = sorted(results, key=lambda x: x[0])
        for _, result in results:
            outf.write("".join(result[-2]))
            if mismatchf is not None:
                mismatchf.write("".join(result[-1]))
        return results


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    parser.add_argument("-r", "--route-files", dest="routeFiles",
                        help="Input route file")
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
    parser.add_argument("--turn-ratio-tolerance", dest="turnRatioTolerance", type=float,
                        help="Set tolerance for error in resulting ratios (relative to turn-ratio-total)")
    parser.add_argument("--turn-ratio-abs-tolerance", dest="turnRatioAbsTolerance", type=int, default=1,
                        help="Set tolerance for error in resulting turning ratios as absolute count")
    parser.add_argument("--turn-max-gap", type=int, dest="turnMaxGap", default=0,
                        help="Allow at most a gap of INT edges between from-edge and to-edge")
    parser.add_argument("--total-count", dest="totalCount",
                        help="Set a total count that should be reached (either as single value that is split "
                             + " proportionally among all intervals or as a list of counts per interval)."
                             + " Setting the value 'input' preserves input vehicle counts in each interval.")
    parser.add_argument("-o", "--output-file", dest="out", default="out.rou.xml",
                        help="Output route file")
    parser.add_argument("--prefix", dest="prefix", default="",
                        help="prefix for the vehicle ids")
    parser.add_argument("-a", "--attributes", dest="vehattrs", default="",
                        help="additional vehicle attributes")
    parser.add_argument("-s", "--seed", type=int, default=42,
                        help="random seed")
    parser.add_argument("--mismatch-output", dest="mismatchOut",
                        help="write cout-data with overflow/underflow information to FILE")
    parser.add_argument("--weighted", dest="weighted", action="store_true", default=False,
                        help="Sample routes according to their probability (or count)")
    parser.add_argument("--optimize",
                        help="set optimization method level (full, INT boundary)")
    parser.add_argument("--optimize-input", dest="optimizeInput", action="store_true", default=False,
                        help="Skip resampling and run optimize directly on the input routes")
    parser.add_argument("--min-count", dest="minCount", type=int, default=1,
                        help="Set minimum number of counting locations that a route must visit")
    parser.add_argument("--minimize-vehicles", dest="minimizeVehs", type=float, default=0,
                        help="Set optimization factor from [0, 1[ for reducing the number of vehicles"
                        + "(prefer routes that pass multiple counting locations over routes that pass fewer)")
    parser.add_argument("--geh-ok", dest="gehOk", type=float, default=5,
                        help="threshold for acceptable GEH values")
    parser.add_argument("-f", "--write-flows", dest="writeFlows",
                        help="write flows with the give style instead of vehicles [number|probability|poisson]")
    parser.add_argument("-I", "--write-route-ids", dest="writeRouteIDs", action="store_true", default=False,
                        help="write routes with ids")
    parser.add_argument("-u", "--write-route-distribution", dest="writeRouteDist",
                        help="write routeDistribution with the given ID instead of individual routes")
    parser.add_argument("--pedestrians", action="store_true", default=False,
                        help="write person walks instead of vehicle routes")
    parser.add_argument("-b", "--begin", help="custom begin time (seconds or H:M:S)")
    parser.add_argument("-e", "--end", help="custom end time (seconds or H:M:S)")
    parser.add_argument("-i", "--interval", help="custom aggregation interval (seconds or H:M:S)")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="tell me what you are doing")
    parser.add_argument("-V", "--verbose.histograms", dest="verboseHistogram", action="store_true", default=False,
                        help="print histograms of edge numbers and detector passing count")
    parser.add_argument("--threads", dest="threads", type=int, default=1,
                        help="If parallelization is desired, enter the number of CPUs to use. Set to a value >> then "
                             "your machines CPUs if you want to utilize all CPUs (Default is 1)"
                        )

    options = parser.parse_args(args=args)
    if (options.routeFiles is None or
            (options.turnFiles is None and options.edgeDataFiles is None and options.odFiles is None)):
        parser.print_help()
        sys.exit()
    if options.writeRouteIDs and options.writeRouteDist:
        sys.stderr.write("Only one of the options --write-route-ids and --write-route-distribution may be used")
        sys.exit()
    if options.writeFlows not in [None, "number", "probability", "poisson"]:
        sys.stderr.write("Options --write-flows only accepts arguments 'number', 'probability' and 'poisson'")
        sys.exit()

    options.routeFiles = options.routeFiles.split(',')
    options.turnFiles = options.turnFiles.split(',') if options.turnFiles is not None else []
    options.edgeDataFiles = options.edgeDataFiles.split(',') if options.edgeDataFiles is not None else []
    options.odFiles = options.odFiles.split(',') if options.odFiles is not None else []
    if options.vehattrs and options.vehattrs[0] != ' ':
        options.vehattrs = ' ' + options.vehattrs

    if options.optimize is not None:
        try:
            import scipy.optimize  # noqa
            if options.optimize != "full":
                try:
                    options.optimize = int(options.optimize)
                except Exception:
                    print("Option optimize requires the value 'full' or an integer", file=sys.stderr)
                    sys.exit(1)
        except ImportError:
            print("Cannot use optimization (scipy not installed)", file=sys.stderr)
            sys.exit(1)

    if options.optimizeInput and type(options.optimize) != int:
        print("Option --optimize-input requires an integer argument for --optimize", file=sys.stderr)
        sys.exit(1)

    if options.threads > 1 and sys.version_info[0] < 3:
        print("Using multiple cpus is only supported for python 3", file=sys.stderr)
        sys.exit(1)

    if options.totalCount:
        if options.totalCount != PRESERVE_INPUT_COUNT:
            options.totalCount = list(map(int, options.totalCount.split(',')))

    return options


class CountData:
    def __init__(self, count, edgeTuple, allRoutes, isOrigin, isDest, isRatio, options):
        self.index = None
        self.origCount = count
        self.count = count
        self.edgeTuple = edgeTuple
        self.isOrigin = isOrigin
        self.isDest = isDest
        self.isRatio = isRatio
        self.ratioSiblings = []
        self.assignedCount = 0
        self.options = options  # multiprocessing had issue with sumolib.options.getOptions().turnMaxGap
        self.routeSet = set()
        for routeIndex, edges in enumerate(allRoutes.unique):
            if self.routePasses(edges) is not None:
                self.routeSet.add(routeIndex)
        if isRatio:
            self.count = options.turnRatioAbsTolerance

    def routePasses(self, edges):

        if self.isOrigin or self.isDest:
            passes = ((not self.isOrigin or edges[0] == self.edgeTuple[0]) and
                      (not self.isDest or edges[-1] == self.edgeTuple[-1]))
            if not passes:
                return None
            else:
                return 0 if self.isOrigin else len(edges) - 1
        i = None
        try:
            i = edges.index(self.edgeTuple[0])
            maxDelta = self.options.turnMaxGap + 1
            for edge in self.edgeTuple[1:]:
                i2 = edges.index(edge, i)
                if i2 - i > maxDelta:
                    return None
                i = i2
        except ValueError:
            # first edge not in route
            return None
        return i

    def use(self):
        self.count -= 1
        self.assignedCount += 1

    def addCount(self, count):
        self.count += count
        self.origCount += count

    def sampleOpen(self, rng, openRoutes, routeCounts):
        cands = list(self.routeSet.intersection(openRoutes))
        assert (cands)
        probs = [routeCounts[i] for i in cands]
        x = rng.rand() * sum(probs)
        seen = 0
        for route, prob in zip(cands, probs):
            seen += prob
            if seen >= x:
                return route
        assert (False)

    def updateTurnRatioCounts(self, openRoutes, openCounts, updateSiblings=False):
        if self.isRatio:
            total = sum([cd2.assignedCount for cd2 in self.ratioSiblings])
            permitted = total * self.origCount + self.options.turnRatioAbsTolerance
            if permitted > self.assignedCount:
                self.count = permitted - self.assignedCount
                for rI in self.routeSet:
                    if rI not in openRoutes:
                        openRoutes.append(rI)
                if self.index not in openCounts:
                    openCounts.append(self.index)
            if updateSiblings:
                for cd2 in self.ratioSiblings:
                    if cd2 != self:
                        cd2.updateTurnRatioCounts(openRoutes, openCounts)

    def __repr__(self):
        return "CountData(edges=%s, count=%s, origCount=%s%s%s%s%s)\n" % (
            self.edgeTuple, self.count, self.origCount,
            ", isOrigin=True" if self.isOrigin else "",
            ", isDest=True" if self.isDest else "",
            ", isRatio=True" if self.isRatio else "",
            (", sibs=%s" % len(self.ratioSiblings)) if self.isRatio else ""
        )


def getIntervals(options):
    begin, end, interval = parseTimeRange(options.turnFiles + options.edgeDataFiles + options.odFiles)
    if options.begin is not None:
        begin = parseTime(options.begin)
    if options.end is not None:
        end = parseTime(options.end)
    if options.interval is not None:
        interval = parseTime(options.interval)

    result = []
    while begin < end:
        result.append((begin, begin + interval))
        begin += interval

    return result


def getOverlap(begin, end, iBegin, iEnd):
    """return overlap of the given intervals as fraction"""
    if iEnd <= begin or end <= iBegin:
        return 0  # no overlap
    elif iBegin >= begin and iEnd <= end:
        return 1  # data interval fully within requested interval
    elif iBegin <= begin and iEnd >= end:
        return (end - begin) / (iEnd - iBegin)  # only part of the data interval applies to the requested interval
    elif iBegin <= begin and iEnd <= end:
        return (iEnd - begin) / (iEnd - iBegin)  # partial overlap
    else:
        return (end - iBegin) / (iEnd - iBegin)  # partial overlap


def parseTurnCounts(interval, attr, warn):
    if interval.edgeRelation is not None:
        for edgeRel in interval.edgeRelation:
            via = [] if edgeRel.via is None else edgeRel.via.split(' ')
            edges = tuple([edgeRel.attr_from] + via + [edgeRel.to])
            value = getattr(edgeRel, attr)
            yield edges, value
    elif warn:
        sys.stderr.write("Warning: No edgeRelations in interval from=%s to=%s\n" % (interval.begin, interval.end))


def parseEdgeCounts(interval, attr, warn):
    if interval.edge is not None:
        for edge in interval.edge:
            yield (edge.id,), getattr(edge, attr)
    elif warn:
        sys.stderr.write("Warning: No edges in interval from=%s to=%s\n" % (interval.begin, interval.end))


def parseDataIntervals(parseFun, fnames, begin, end, allRoutes, attr, options,
                       isOrigin=False, isDest=False, isRatio=False, warn=False):
    locations = {}  # edges -> CountData
    result = []
    if attr is None or attr == "None":
        return result
    for fname in fnames:
        for interval in sumolib.xml.parse(fname, 'interval', heterogeneous=True):
            overlap = 1 if isRatio else getOverlap(begin, end, parseTime(interval.begin), parseTime(interval.end))
            if overlap > 0:
                # print(begin, end, interval.begin, interval.end, "overlap:", overlap)
                for edges, value in parseFun(interval, attr, warn):
                    try:
                        value = float(value)
                    except TypeError:
                        if warn:
                            print("Warning: Missing '%s' value in file '%s' for edge(s) '%s'" %
                                  (attr, fname, ' '.join(edges)), file=sys.stderr)
                        continue
                    if edges not in locations:
                        result.append(CountData(0, edges, allRoutes, isOrigin, isDest, isRatio, options))
                        locations[edges] = result[-1]
                    elif (isOrigin and isDest) != (locations[edges].isOrigin and locations[edges].isDest):
                        print("Warning: Edge relation '%s' occurs as turn relation and also as OD-relation" %
                              ' '.join(edges), file=sys.stderr)
                    elif isRatio != locations[edges].isRatio:
                        print("Warning: Edge relation '%s' occurs as turn relation and also as turn-ratio" %
                              ' '.join(edges), file=sys.stderr)
                    value *= overlap
                    if not isRatio:
                        value = int(value)
                    locations[edges].addCount(value)
    return result


def parseTimeRange(fnames):
    begin = 1e20
    end = 0
    minInterval = 1e20
    for fname in fnames:
        for interval in sumolib.xml.parse(fname, 'interval'):
            iBegin = parseTime(interval.begin)
            iEnd = parseTime(interval.end)
            begin = min(begin, iBegin)
            end = max(end, iEnd)
            minInterval = min(minInterval, iEnd - iBegin)
    return begin, end, minInterval


def hasCapacity(dataIndices, countData):
    for i in dataIndices:
        if countData[i].count <= 0:
            return False
    return True


def updateOpenRoutes(openRoutes, routeUsage, countData):
    return list(filter(lambda r: hasCapacity(routeUsage[r], countData), openRoutes))


def updateOpenCounts(openCounts, countData, openRoutes):
    return list(filter(lambda i: countData[i].routeSet.intersection(openRoutes), openCounts))


def optimize(options, countData, routes, usedRoutes, routeUsage, intervalCount):
    """ use relaxtion of the ILP problem for picking the number of times that each route is used
    x = usageCount vector (count for each route index)
    c = weight vector (vector of 1s)
    A_eq = routeUsage encoding
    b_eq = counts

    Rationale:
      c: costs for using each route,
         when minimizing x @ c, routes that pass multiple counting stations are getting an advantage

    """
    import scipy.optimize as opt

    m = len(countData)

    priorRouteCounts = getRouteCounts(routes, usedRoutes)

    relevantRoutes = [i for i in range(routes.number) if len(routeUsage[i]) >= options.minCount]
    priorRelevantRouteCounts = [priorRouteCounts[r] for r in relevantRoutes]
    k = len(relevantRoutes)

    if options.optimize == "full":
        # allow changing all prior usedRoutes
        bounds = None
    else:
        u = int(options.optimize)
        if u == 0:
            print("Optimization skipped")
            return
        # limited optimization: change prior routeCounts by at most u per route
        bounds = [(max(0, p - u), p + u) for p in priorRelevantRouteCounts] + [(0, None)] * m

    # Ax <= b
    # Ax + s = b
    # min s
    # -> x2 = [x, s]

    A = np.zeros((m, k))
    for i in range(0, m):
        for j in range(0, k):
            A[i][j] = int(relevantRoutes[j] in countData[i].routeSet)
    A_eq = np.concatenate((A, np.identity(m)), 1)

    # constraint: achieve counts
    b = np.asarray([cd.origCount for cd in countData])

    # minimization objective [routeCounts] + [slack]
    c = [options.minimizeVehs] * k + [1] * m

    A_ub = None
    b_ub = None

    if intervalCount is not None:
        # add inequality to ensure that we stay below intervalCount
        # sum of routes < intervalCount
        # A_ub * x <= b_ub
        A_ub = np.concatenate((np.ones((1, k)), np.zeros((1, m))), 1)
        b_ub = np.asarray([intervalCount])

    # print("k=%s" % k)
    # print("m=%s" % m)
    # print("A_eq (%s) %s" % (A_eq.shape, A_eq))
    # print("b (%s) %s" % (len(b), b))
    # print("c (%s) %s" % (len(c), c))
    # print("bounds (%s) %s" % (len(bounds) if bounds is not None else "-", bounds))

    linProgOpts = {}
    if options.verbose:
        linProgOpts["disp"] = True

    res = opt.linprog(c, A_ub=A_ub, b_ub=b_ub, A_eq=A_eq, b_eq=b, bounds=bounds, options=linProgOpts)

    if res.success:
        print("Optimization succeeded")
        del usedRoutes[:]
        routeCountsR = res.x[:k]  # cut of slack variables
        # translate to original route indices
        routeCounts = [0] * routes.number
        for index, count in zip(relevantRoutes, routeCountsR):
            routeCounts[index] = count

        # print("priorRouteCounts", priorRouteCounts)
        # print("relevantRoutes", relevantRoutes)
        # print("priorRelevantRouteCounts", priorRelevantRouteCounts)
        # print("routeCountsR", routeCountsR)
        # print("routeCounts", routeCounts)
        # slack = res.x[k:]
        # print("routeCounts (n=%s, sum=%s, intSum=%s, roundSum=%s) %s" % (
        #    len(routeCounts),
        #    sum(routeCounts),
        #    sum(map(int, routeCounts)),
        #    sum(map(round, routeCounts)),
        #    routeCounts))
        # print("slack (n=%s, sum=%s) %s" % (len(slack), sum(slack), slack))
        usedRoutes.extend(sum([[i] * int(round(count)) for i, count in enumerate(routeCounts)], []))
        # print("#usedRoutes=%s" % len(usedRoutes))
        # update countData
    else:
        print("Optimization failed")


def zero():
    # cannot pickle lambda for multiprocessing
    return 0


class Routes:
    def __init__(self, routefiles):
        self.all = []
        self.edgeProbs = defaultdict(zero)
        self.edgeIDs = {}
        self.withProb = 0
        for routefile in routefiles:
            warned = False
            # not all routes may have specified probability, in this case use their number of occurrences
            for r in sumolib.xml.parse(routefile, ['route', 'walk'], heterogeneous=True):
                if r.edges is None:
                    if not warned:
                        print("Warning: Ignoring walk in file '%s' because it does not contain edges." % routefile,
                              file=sys.stderr)
                        warned = True
                    continue
                edges = tuple(r.edges.split())
                self.all.append(edges)
                prob = float(r.getAttributeSecure("probability", 1))
                if r.hasAttribute("probability"):
                    self.withProb += 1
                    prob = float(r.probability)
                else:
                    prob = 1
                if prob <= 0:
                    print("Warning: route probability must be positive (edges=%s)" % r.edges, file=sys.stderr)
                    prob = 0
                if r.hasAttribute("id"):
                    self.edgeIDs[edges] = r.id
                self.edgeProbs[edges] += prob
        self.unique = sorted(list(self.edgeProbs.keys()))
        self.number = len(self.unique)
        self.edges2index = dict([(e, i) for i, e in enumerate(self.unique)])
        if len(self.unique) == 0:
            print("Error: no input routes loaded", file=sys.stderr)
            sys.exit()
        self.probabilities = np.array([self.edgeProbs[e] for e in self.unique], dtype=np.float64)


def initTurnRatioSiblings(routes, countData):
    ratioIndices = set()
    # todo: use routes to complete incomplete sibling lists
    ratioOrigins = defaultdict(list)
    for cd in countData:
        if cd.isRatio:
            ratioOrigins[cd.edgeTuple[0]].append(cd)
            ratioIndices.add(cd.index)

    for edge, cDs in ratioOrigins.items():
        for cd in cDs:
            cd.ratioSiblings = cDs
    return ratioIndices


def resetCounts(usedRoutes, routeUsage, countData):
    for cd in countData:
        cd.count = cd.origCount
        cd.assignedCount = 0
    for r in usedRoutes:
        for i in routeUsage[r]:
            countData[i].use()


def getRouteCounts(routes, usedRoutes):
    result = [0] * routes.number
    for r in usedRoutes:
        result[r] += 1
    return result


def getRouteUsage(routes, countData):
    """store which counting locations are used by each route (using countData index)"""
    routeUsage = [set() for r in routes.unique]
    for i, cd in enumerate(countData):
        for routeIndex in cd.routeSet:
            routeUsage[routeIndex].add(i)
    return routeUsage


def initTotalCounts(options, routes, intervals, b, e):
    """initialize time line for options.totalCount if the number of input vehicles/persons should be preserved
    or in case a single value was given for multiple intervals
    """
    if options.totalCount == PRESERVE_INPUT_COUNT:
        element = 'person' if options.pedestrians else 'vehicle'
        options.totalCount = [0] * len(intervals)
        interval = e - b if len(intervals) == 1 else intervals[0][1] - intervals[0][0]
        numVehs = 0
        numExcluded = 0
        for routefile in options.routeFiles:
            for veh in sumolib.xml.parse(routefile, [element], heterogeneous=True):
                numVehs += 1
                depart = parseTime(veh.depart)
                if depart >= b and depart <= e:
                    iIndex = int((depart - b) / interval)
                    if depart == e:
                        iIndex -= 1
                    assert(iIndex) < len(options.totalCount)
                    options.totalCount[iIndex] += 1
                else:
                    numExcluded += 1
        if options.verbose:
            if len(intervals) == 1:
                print("Using total count of %s corresponding to input %s count" % (
                      numVehs - numExcluded, element))
            else:
                print("Using total count of %s in proportion to input %s counts: %s" % (
                      numVehs - numExcluded, element, ','.join(map(str, options.totalCount))))
        if numExcluded > 0:
            print("Ignored %s %ss because they depart outside the configured time range [%s, %s]" %
                  (numExcluded, element, humanReadableTime(b), humanReadableTime(e)),
                  file=sys.stderr)

    elif len(options.totalCount) != len(intervals):
        if len(options.totalCount) == 1:
            # split proportionally
            countSums = []
            for begin, end in intervals:
                countData = parseCounts(options, routes, begin, end)
                countSums.append(sum(cd.origCount for cd in countData))
            countSumTotal = sum(countSums)
            origTotal = options.totalCount[0]
            options.totalCount = [int(ceil(origTotal * s / countSumTotal)) for s in countSums]
            if options.verbose:
                print("Splitting total count of %s in proportion to interval counting data: %s" % (
                    origTotal, ','.join(map(str, options.totalCount))))
        else:
            sys.stderr.write("Error: --total-count must be a single value" +
                             " or match the number of data intervals (%s)" % len(intervals))
            sys.exit()


def main(options):
    rng = np.random.RandomState(options.seed)

    routes = Routes(options.routeFiles)

    intervals = getIntervals(options)
    if len(intervals) == 0:
        print("Error: no intervals loaded", file=sys.stderr)
        sys.exit()

    # preliminary integrity check for the whole time range
    b = intervals[0][0]
    e = intervals[-1][-1]
    countData = parseCounts(options, routes, b, e, True)
    routeUsage = getRouteUsage(routes, countData)

    for cd in countData:
        if cd.count > 0 and not cd.routeSet:
            msg = ""
            if cd.isOrigin and cd.isDest:
                msg = "start at edge '%s' and end at edge '%s'" % (cd.edgeTuple[0], cd.edgeTuple[-1])
            elif cd.isOrigin:
                msg = "start at edge '%s'" % cd.edgeTuple[0]
            elif cd.isDest:
                msg = "end at edge '%s'" % cd.edgeTuple[-1]
            elif len(cd.edgeTuple) > 1:
                msg = "pass edges '%s'" % ' '.join(cd.edgeTuple)
            else:
                msg = "pass edge '%s'" % ' '.join(cd.edgeTuple)
            print("Warning: no routes %s (count %s)" % (msg, cd.count), file=sys.stderr)

    if options.verbose:
        print("Loaded %s routes (%s distinct)" % (len(routes.all), routes.number))
        if options.weighted:
            print("Loaded probability for %s routes" % routes.withProb)
        if options.verboseHistogram:
            edgeCount = sumolib.miscutils.Statistics("route edge count", histogram=True)
            detectorCount = sumolib.miscutils.Statistics("route detector count", histogram=True)
            for i, edges in enumerate(routes.unique):
                edgeCount.add(len(edges), i)
                detectorCount.add(len(routeUsage[i]), i)
            print("input %s" % edgeCount)
            print("input %s" % detectorCount)

    mismatchf = None
    if options.mismatchOut:
        mismatchf = open(options.mismatchOut, 'w')
        sumolib.writeXMLHeader(mismatchf, "$Id$", options=options)  # noqa
        mismatchf.write('<data>\n')

    if options.totalCount:
        initTotalCounts(options, routes, intervals, b, e)

    underflowSummary = sumolib.miscutils.Statistics("avg interval underflow")
    overflowSummary = sumolib.miscutils.Statistics("avg interval overflow")
    gehSummary = sumolib.miscutils.Statistics("avg interval GEH%")
    inputCountSummary = sumolib.miscutils.Statistics("avg interval input count")
    usedRoutesSummary = sumolib.miscutils.Statistics("avg interval written vehs")

    with open(options.out, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", options=options)  # noqa
        if options.threads > 1:
            # call the multiprocessing function
            results = multi_process(options.threads, intervals,
                                    _solveIntervalMP, outf, mismatchf, options=options, routes=routes)
            # handle the uFlow, oFlow and GEH
            for _, result in results:
                for i, begin in enumerate(result[0]):
                    underflowSummary.add(result[1][i], begin)
                    overflowSummary.add(result[2][i], begin)
                    gehSummary.add(result[3][i], begin)
                    inputCountSummary.add(result[4][i], begin)
                    usedRoutesSummary.add(result[5][i], begin)
        else:
            for i, (begin, end) in enumerate(intervals):
                intervalPrefix = "" if len(intervals) == 1 else "%s_" % int(begin)
                intervalCount = options.totalCount[i] if options.totalCount else None
                uFlow, oFlow, gehOK, inputCount, usedRoutes, _ = solveInterval(options, routes, begin, end,
                                                                               intervalPrefix, outf, mismatchf, rng,
                                                                               intervalCount)
                underflowSummary.add(uFlow, begin)
                overflowSummary.add(oFlow, begin)
                gehSummary.add(gehOK, begin)
                inputCountSummary.add(inputCount, begin)
                usedRoutesSummary.add(usedRoutes, begin)
        outf.write('</routes>\n')

    if options.mismatchOut:
        mismatchf.write('</data>\n')
        mismatchf.close()

    if len(intervals) > 1:
        print(inputCountSummary)
        print(usedRoutesSummary)
        print(underflowSummary)
        print(overflowSummary)
        print(gehSummary)


def _sample_skewed(sampleSet, rng, probabilityMap):
    # build cumulative distribution function for weighted sampling
    cdf = []
    total = 0
    population = tuple(sampleSet)
    for element in population:
        total += probabilityMap[element]
        cdf.append(total)

    value = rng.rand() * total
    return population[np.searchsorted(cdf, value)]


def _solveIntervalMP(options, routes, interval, cpuIndex):
    output_list = []
    rng = np.random.RandomState(options.seed + cpuIndex)
    for i, (begin, end) in enumerate(interval):
        local_outf = StringIO()
        local_mismatch_outf = StringIO() if options.mismatchOut else None
        intervalPrefix = "%s_" % int(begin)
        intervalCount = options.totalCount[i] if options.totalCount else None
        uFlow, oFlow, gehOKNum, inputCount, usedRoutes, local_outf = solveInterval(
            options, routes, begin, end, intervalPrefix, local_outf, local_mismatch_outf, rng, intervalCount)

        output_list.append([begin, uFlow, oFlow, gehOKNum, inputCount, usedRoutes, local_outf.getvalue(),
                            local_mismatch_outf.getvalue() if options.mismatchOut else None])
    output_lst = list(zip(*output_list))
    return output_lst


def parseCounts(options, routes, b, e, warn=False):
    countData = (parseDataIntervals(parseTurnCounts, options.turnFiles, b, e,
                                    routes, options.turnAttr, options=options, warn=warn)
                 + parseDataIntervals(parseTurnCounts, options.turnFiles, b, e,
                                      routes, options.turnRatioAttr, options=options, isRatio=True, warn=warn)
                 + parseDataIntervals(parseEdgeCounts, options.edgeDataFiles, b, e,
                                      routes, options.edgeDataAttr, options=options, warn=warn)
                 + parseDataIntervals(parseTurnCounts, options.odFiles, b, e,
                                      routes, options.turnAttr, options=options,
                                      isOrigin=True, isDest=True, warn=warn)
                 + parseDataIntervals(parseEdgeCounts, options.edgeDataFiles, b, e,
                                      routes, options.departAttr, options=options, isOrigin=True, warn=warn)
                 + parseDataIntervals(parseEdgeCounts, options.edgeDataFiles, b, e,
                                      routes, options.arrivalAttr, options=options, isDest=True, warn=warn)
                 )
    for i, cd in enumerate(countData):
        cd.index = i
    return countData


def solveInterval(options, routes, begin, end, intervalPrefix, outf, mismatchf, rng, intervalCount):
    countData = parseCounts(options, routes, begin, end)

    ratioIndices = None
    if options.turnFiles and options.turnRatioAttr:
        ratioIndices = initTurnRatioSiblings(routes, countData)

    routeUsage = getRouteUsage(routes, countData)
    unrestricted_list = [r for r, usage in enumerate(routeUsage) if len(usage) < options.minCount]
    unrestricted = set(unrestricted_list)
    if options.verbose and len(unrestricted) > 0:
        if options.minCount == 1:
            print("Ignored %s routes which do not pass any counting location" % len(unrestricted))
        else:
            print("Ignored %s routes which pass fewer than %s counting location" % (
                len(unrestricted), options.minCount))

    # pick a random counting location and select a new route that passes it until
    # all counts are satisfied or no routes can be used anymore

    openRoutes = updateOpenRoutes(range(0, routes.number), routeUsage, countData)
    openRoutes = [r for r in openRoutes if r not in unrestricted]
    openCounts = updateOpenCounts(range(0, len(countData)), countData, openRoutes)

    usedRoutes = []
    numSampled = 0
    if options.optimizeInput:
        usedRoutes = [routes.edges2index[e] for e in routes.all]
        resetCounts(usedRoutes, routeUsage, countData)
    elif options.optimize != "full":
        while openCounts:
            if intervalCount is not None and numSampled >= intervalCount:
                break

            if ratioIndices and intervalCount is None:
                realCounts = [cdi for cdi in openCounts if cdi not in ratioIndices]
                if not realCounts:
                    print("Stopped sampling routes because only ratios are still open."
                          + " Set option --total-count to sample ratios without local counts",
                          file=sys.stderr)
                    break

            if options.weighted:
                routeIndex = _sample_skewed(openRoutes, rng, routes.probabilities)
            else:
                # sampling equally among open counting locations appears to
                # improve GEH but it would also introduce a bias in the loaded
                # route probabilities
                cd = countData[rng.choice(openCounts)]
                routeIndex = rng.choice([r for r in openRoutes if r in cd.routeSet])
            numSampled += 1
            usedRoutes.append(routeIndex)
            for dataIndex in routeUsage[routeIndex]:
                countData[dataIndex].use()

            if ratioIndices:
                for dataIndex in routeUsage[routeIndex]:
                    countData[dataIndex].updateTurnRatioCounts(openRoutes, openCounts, True)

                # this is the old and slow way to update things
                openRoutes = updateOpenRoutes(openRoutes, routeUsage, countData)
                openCounts = updateOpenCounts(openCounts, countData, openRoutes)

            else:
                # update openRouts and openCounts only if needed
                closedRoutes = set()
                for dataIndex in routeUsage[routeIndex]:
                    cd = countData[dataIndex]
                    if cd.count == 0:
                        openCounts.remove(dataIndex)
                        for r in cd.routeSet:
                            closedRoutes.add(r)

                if closedRoutes:
                    cdRecheck = set()
                    openRoutes2 = []
                    for r in openRoutes:
                        if r in closedRoutes:
                            for dataIndex in routeUsage[r]:
                                cdRecheck.add(dataIndex)
                        else:
                            openRoutes2.append(r)
                    openRoutes = openRoutes2
                    closedCounts = [c for c in cdRecheck if not countData[c].routeSet.intersection(openRoutes)]
                    if closedCounts:
                        openCounts = [c for c in openCounts if c not in closedCounts]

    totalMismatch = sum([cd.count for cd in countData])  # noqa

    if totalMismatch > 0 and options.optimize is not None:
        if options.verbose:
            print("Starting optimization for interval [%s, %s] (mismatch %s)" % (
                begin, end, totalMismatch))
        optimize(options, countData, routes, usedRoutes, routeUsage, intervalCount)
        resetCounts(usedRoutes, routeUsage, countData)
        numSampled = len(usedRoutes)

    if intervalCount is not None and numSampled < intervalCount:
        if unrestricted:
            if options.minCount != 1:
                # ensure that we only sample from routes that do not pass any counting locations
                unrestricted_list = [r for r, usage in enumerate(routeUsage) if len(usage) == 0]
                unrestricted = set(unrestricted_list)

            while numSampled < intervalCount:
                if options.weighted:
                    routeIndex = _sample_skewed(unrestricted, rng, routes.probabilities)
                else:
                    routeIndex = rng.choice(unrestricted_list)
                usedRoutes.append(routeIndex)
                assert(len(routeUsage[routeIndex]) == 0)
                numSampled += 1
        else:
            print("Cannot fulfill total interval count of %s due to lack of unrestricted routes" % intervalCount,
                  file=sys.stderr)

    # avoid bias from sampling order / optimization
    rng.shuffle(usedRoutes)

    if usedRoutes:
        outf.write('<!-- begin="%s" end="%s" -->\n' % (begin, end))
        period = (end - begin) / len(usedRoutes)
        depart = begin
        routeCounts = getRouteCounts(routes, usedRoutes)
        if options.writeRouteIDs:
            for routeIndex in sorted(set(usedRoutes)):
                edges = routes.unique[routeIndex]
                routeIDComment = ""
                if edges in routes.edgeIDs:
                    routeIDComment = " (%s)" % routes.edgeIDs[edges]
                outf.write('    <route id="%s%s%s" edges="%s"/> <!-- %s%s -->\n' % (
                    options.prefix, intervalPrefix, routeIndex, ' '.join(edges),
                    routeCounts[routeIndex], routeIDComment))
            outf.write('\n')
        elif options.writeRouteDist:
            outf.write('    <routeDistribution id="%s%s%s">\n' % (
                       options.prefix, intervalPrefix, options.writeRouteDist))
            for routeIndex in sorted(set(usedRoutes)):
                outf.write('        <route id="%s%s%s" edges="%s" probability="%s"/>\n' % (
                    options.prefix, intervalPrefix, routeIndex,
                    ' '.join(routes.unique[routeIndex]), routeCounts[routeIndex]))
            outf.write('    </routeDistribution>\n\n')

        routeID = options.writeRouteDist
        if options.writeFlows is None:
            for i, routeIndex in enumerate(usedRoutes):
                if options.writeRouteIDs:
                    routeID = routeIndex
                vehID = options.prefix + intervalPrefix + str(i)
                if routeID is not None:
                    if options.pedestrians:
                        outf.write('    <person id="%s" depart="%.2f"%s>\n' % (
                            vehID, depart, options.vehattrs))
                        outf.write('        <walk route="%s%s%s"/>\n' % (options.prefix, intervalPrefix, routeID))
                        outf.write('    </person>\n')
                    else:
                        outf.write('    <vehicle id="%s" depart="%.2f" route="%s%s%s"%s/>\n' % (
                            vehID, depart, options.prefix, intervalPrefix, routeID, options.vehattrs))
                else:
                    if options.pedestrians:
                        outf.write('    <person id="%s" depart="%.2f"%s>\n' % (
                            vehID, depart, options.vehattrs))
                        outf.write('        <walk edges="%s"/>\n' % ' '.join(routes.unique[routeIndex]))
                        outf.write('    </person>\n')
                    else:
                        outf.write('    <vehicle id="%s" depart="%.2f"%s>\n' % (
                            vehID, depart, options.vehattrs))
                        outf.write('        <route edges="%s"/>\n' % ' '.join(routes.unique[routeIndex]))
                        outf.write('    </vehicle>\n')
                depart += period
        else:
            routeDeparts = defaultdict(list)
            for routeIndex in usedRoutes:
                routeDeparts[routeIndex].append(depart)
                depart += period
            if options.writeRouteDist:
                totalCount = sum(routeCounts)
                probability = totalCount / (end - begin)
                flowID = options.prefix + intervalPrefix + options.writeRouteDist
                if options.writeFlows == "poisson":
                    repeat = 'period="exp(%.4f)"' % probability
                elif options.writeFlows == "number" or probability > 1.00004:
                    repeat = 'number="%s"' % totalCount
                    if options.writeFlows == "probability":
                        sys.stderr.write("Warning: could not write flow %s with probability %.5f\n" %
                                         (flowID, probability))
                else:
                    repeat = 'probability="%.4f"' % probability
                outf.write('    <flow id="%s" begin="%.2f" end="%.2f" %s route="%s"%s/>\n' % (
                    flowID, begin, end, repeat,
                    options.writeRouteDist, options.vehattrs))
            else:
                # ensure flows are sorted
                flows = []
                for routeIndex in sorted(set(usedRoutes)):
                    outf2 = StringIO()
                    fBegin = min(routeDeparts[routeIndex])
                    fEnd = max(routeDeparts[routeIndex] + [fBegin + 1.0])
                    probability = routeCounts[routeIndex] / (fEnd - fBegin)
                    flowID = "%s%s%s" % (options.prefix, intervalPrefix, routeIndex)
                    if options.writeFlows == "poisson":
                        repeat = 'period="exp(%.4f)"' % probability
                    elif options.writeFlows == "number" or probability > 1.00004:
                        repeat = 'number="%s"' % routeCounts[routeIndex]
                        if options.writeFlows == "probability":
                            sys.stderr.write("Warning: could not write flow %s with probability %.5f\n" % (
                                flowID, probability))
                    else:
                        repeat = 'probability="%.4f"' % probability
                    if options.writeRouteIDs:
                        if options.pedestrians:
                            outf2.write('    <personFlow id="%s" begin="%.2f" end="%.2f" %s%s>\n' % (
                                flowID, fBegin, fEnd, repeat, options.vehattrs))
                            outf2.write('        <walk route="%s%s%s"/>\n' % (
                                options.prefix, intervalPrefix, routeIndex))
                            outf2.write('    </personFlow>\n')
                        else:
                            outf2.write('    <flow id="%s" begin="%.2f" end="%.2f" %s route="%s%s%s"%s/>\n' % (
                                flowID, fBegin, fEnd, repeat,
                                options.prefix, intervalPrefix, routeIndex, options.vehattrs))
                    else:
                        if options.pedestrians:
                            outf2.write('    <personFlow id="%s" begin="%.2f" end="%.2f" %s%s>\n' % (
                                flowID, fBegin, fEnd, repeat, options.vehattrs))
                            outf2.write('        <walk edges="%s"/>\n' % ' '.join(routes.unique[routeIndex]))
                            outf2.write('    </personFlow>\n')
                        else:
                            outf2.write('    <flow id="%s" begin="%.2f" end="%.2f" %s%s>\n' % (
                                flowID, fBegin, fEnd, repeat, options.vehattrs))
                            outf2.write('        <route edges="%s"/>\n' % ' '.join(routes.unique[routeIndex]))
                            outf2.write('    </flow>\n')
                    flows.append((fBegin, outf2))
                flows.sort()
                for fBegin, outf2 in flows:
                    outf.write(outf2.getvalue())

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
    print("%sWrote %s routes (%s distinct) achieving total count %s (%s) at %s locations. GEH<%s for %s" % (
        outputIntervalPrefix,
        len(usedRoutes), len(set(usedRoutes)),
        totalCount, countPercentage, len(countData),
        options.gehOk, gehOK))

    if options.verboseHistogram:
        edgeCount = sumolib.miscutils.Statistics("route edge count", histogram=True)
        detectorCount = sumolib.miscutils.Statistics("route detector count", histogram=True)
        for i, r in enumerate(usedRoutes):
            edgeCount.add(len(routes.unique[r]), i)
            detectorCount.add(len(routeUsage[r]), i)
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

    return sum(underflow.values), sum(overflow.values), gehOKNum, totalOrigCount, len(usedRoutes), outf


if __name__ == "__main__":
    main(get_options())
