#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
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
Samples routes from a given set to fulfill specified counting data (edge counts or turn counts)
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict
from math import ceil, modf
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
from sumolib.statistics import setPrecision  # noqa

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
    op = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    # input
    op.add_argument("-r", "--route-files", category="input", dest="routeFiles", type=op.route_file_list,
                    help="Input route file")
    op.add_argument("-t", "--turn-files", category="input", dest="turnFiles", type=op.file_list,
                    help="Input turn-count file")
    op.add_argument("-T", "--turn-ratio-files", category="input", dest="turnRatioFiles", type=op.file_list,
                    help="Input turn-ratio file")
    op.add_argument("-d", "--edgedata-files", category="input", dest="edgeDataFiles", type=op.file_list,
                    help="Input edgeData file (for counts)")
    op.add_argument("-O", "--od-files", category="input", dest="odFiles", type=op.file_list,
                    help="Input edgeRelation and tazRelation files for origin-destination counts")
    op.add_argument("--taz-files", category="input", dest="tazFiles", type=op.file_list,
                    help="Input TAZ (district) definitions for interpreting tazRelation files")
    op.add_argument("--edgedata-attribute", category="input", dest="edgeDataAttr", default="entered",
                    help="Read edgeData counts from the given attribute")
    op.add_argument("--arrival-attribute", category="input", dest="arrivalAttr",
                    help="Read arrival counts from the given edgeData file attribute")
    op.add_argument("--depart-attribute", category="input", dest="departAttr",
                    help="Read departure counts from the given edgeData file attribute")
    op.add_argument("--turn-attribute", category="input", dest="turnAttr", default="count",
                    help="Read turning counts and origin-destination counts from the given attribute")
    op.add_argument("--turn-ratio-attribute", category="input", dest="turnRatioAttr", default="probability",
                    help="Read turning ratios from the given attribute")
    # output
    op.add_argument("-o", "--output-file", category="output", dest="out", default="out.rou.xml", type=op.route_file,
                    help="Output route file")
    op.add_argument("--mismatch-output", category="output", dest="mismatchOut",
                    help="write cout-data with overflow/underflow information to FILE")
    op.add_argument("--precision", category="output", type=int, dest="precision", default=2,
                    help="Number of decimal digits in output")
    op.add_argument("--keep-stops", category="output", dest="keepStops", action="store_true", default=False,
                    help="Preserve stops from the input routes")
    op.add_argument("-v", "--verbose", category="output", action="store_true", default=False,
                    help="tell me what you are doing")
    op.add_argument("-V", "--verbose.histograms", category="output", dest="verboseHistogram", action="store_true",
                    default=False, help="print histograms of edge numbers and detector passing count")
    # attributes
    op.add_argument("--prefix", category="attributes", dest="prefix", default="",
                    help="prefix for the vehicle ids")
    op.add_argument("-a", "--attributes", category="attributes", dest="vehattrs", default="",
                    help="additional vehicle attributes")
    op.add_argument("-f", "--write-flows", category="attributes", dest="writeFlows",
                    help="write flows with the give style instead of vehicles [number|probability|poisson]")
    op.add_argument("-I", "--write-route-ids", category="attributes", dest="writeRouteIDs", action="store_true",
                    default=False, help="write routes with ids")
    op.add_argument("-u", "--write-route-distribution", category="attributes", dest="writeRouteDist",
                    help="write routeDistribution with the given ID instead of individual routes")
    op.add_argument("--pedestrians", category="attributes", action="store_true", default=False,
                    help="write person walks instead of vehicle routes")
    # time
    op.add_argument("-b", "--begin", category="time",
                    help="custom begin time (seconds or H:M:S)")
    op.add_argument("-e", "--end", category="time",
                    help="custom end time (seconds or H:M:S)")
    op.add_argument("-i", "--interval", category="time",
                    help="custom aggregation interval (seconds or H:M:S)")
    # processing
    op.add_argument("--turn-max-gap", type=int, dest="turnMaxGap", default=0,
                    help="Allow at most a gap of INT edges between from-edge and to-edge")
    op.add_argument("--total-count", dest="totalCount",
                    help="Set a total count that should be reached (either as single value that is split " +
                    " proportionally among all intervals or as a list of counts per interval)." +
                    " Setting the value 'input' preserves input vehicle counts in each interval.")
    op.add_argument("--extra-od", dest="extraOD", action="store_true", default=False,
                    help="Permit traffic between OD-pairs that did not occur in the input")
    op.add_argument("-s", "--seed", type=int, default=42,
                    help="random seed")
    op.add_argument("--weighted", dest="weighted", action="store_true", default=False,
                    help="Sample routes according to their probability (or count)")
    op.add_argument("--optimize",
                    help="set optimization method level (full, INT boundary)")
    op.add_argument("--optimize-input", dest="optimizeInput", action="store_true", default=False,
                    help="Skip resampling and run optimize directly on the input routes")
    op.add_argument("--min-count", dest="minCount", type=int, default=1,
                    help="Set minimum number of counting locations that a route must visit")
    op.add_argument("--minimize-vehicles", dest="minimizeVehs", type=float, default=0,
                    help="Set optimization factor from [0, 1[ for reducing the number of vehicles" +
                    "(prefer routes that pass multiple counting locations over routes that pass fewer)")
    op.add_argument("--geh-ok", dest="gehOk", type=float, default=5,
                    help="threshold for acceptable GEH values")
    op.add_argument("--turn-ratio-total", dest="turnRatioTotal", type=float, default=1,
                    help="Set value for normalizing turning ratios (default 1)")
    op.add_argument("--turn-ratio-tolerance", dest="turnRatioTolerance", type=float,
                    help="Set tolerance for error in resulting ratios (relative to turn-ratio-total)")
    op.add_argument("--turn-ratio-abs-tolerance", dest="turnRatioAbsTolerance", type=int,
                    default=1, help="Set tolerance for error in resulting turning ratios as absolute count")
    op.add_argument("--threads", dest="threads", type=int, default=1,
                    help="If parallelization is desired, enter the number of CPUs to use. Set to a value >> then " +
                    "your machines CPUs if you want to utilize all CPUs (Default is 1)")

    options = op.parse_args(args=args)
    if (options.routeFiles is None or
            (options.turnFiles is None
             and options.turnRatioFiles is None
             and options.edgeDataFiles is None
             and options.odFiles is None)):
        op.print_help()
        sys.exit()
    if options.writeRouteIDs and options.writeRouteDist:
        sys.stderr.write("Only one of the options --write-route-ids and --write-route-distribution may be used")
        sys.exit()
    if options.writeFlows not in [None, "number", "probability", "poisson"]:
        sys.stderr.write("Options --write-flows only accepts arguments 'number', 'probability' and 'poisson'")
        sys.exit()

    options.routeFiles = options.routeFiles.split(',')
    options.turnFiles = options.turnFiles.split(',') if options.turnFiles is not None else []
    options.turnRatioFiles = options.turnRatioFiles.split(',') if options.turnRatioFiles is not None else []
    options.edgeDataFiles = options.edgeDataFiles.split(',') if options.edgeDataFiles is not None else []
    options.odFiles = options.odFiles.split(',') if options.odFiles is not None else []
    options.tazFiles = options.tazFiles.split(',') if options.tazFiles is not None else []
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

    if options.optimizeInput and not isinstance(options.optimize, int):
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
    def __init__(self, count, edgeTuple, allRoutes, isOrigin, isDest, isRatio, isTaz, options):
        self.index = None
        self.origCount = count
        self.count = count
        self.edgeTuple = edgeTuple
        self.isOrigin = isOrigin
        self.isDest = isDest
        self.isRatio = isRatio
        self.isTaz = isTaz
        self.ratioSiblings = []
        self.assignedCount = 0
        self.options = options  # multiprocessing had issue with sumolib.options.getOptions().turnMaxGap
        self.routeSet = set()
        for routeIndex, edges in enumerate(allRoutes.unique):
            if self.routePasses(edges) is not None:
                self.routeSet.add(routeIndex)

    def routePasses(self, edges):
        if self.isTaz:
            if (inTaz(self.options, edges[0], self.edgeTuple[0], True) and
                    inTaz(self.options, edges[-1], self.edgeTuple[-1], False)):
                return 0
            else:
                return None
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
        assert cands
        probs = [routeCounts[i] for i in cands]
        x = rng.rand() * sum(probs)
        seen = 0
        for route, prob in zip(cands, probs):
            seen += prob
            if seen >= x:
                return route
        assert False

    def updateTurnRatioCounts(self, openRoutes, openCounts, updateSiblings=False):
        if self.isRatio:
            total = self.getSiblingCount()
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

    def getSiblingCount(self):
        if not self.isRatio:
            return None
        return sum([cd2.assignedCount for cd2 in self.ratioSiblings])

    def assignedProbability(self):
        if not self.isRatio:
            return None
        sibCount = self.getSiblingCount()
        return 0 if sibCount == 0 else self.assignedCount / sibCount

    def __repr__(self):
        return "CountData(edges=%s, count=%s, origCount=%s%s%s%s%s)\n" % (
            self.edgeTuple, self.count, self.origCount,
            ", isOrigin=True" if self.isOrigin else "",
            ", isDest=True" if self.isDest else "",
            ", isRatio=True" if self.isRatio else "",
            (", sibs=%s" % len(self.ratioSiblings)) if self.isRatio else "")


def inTaz(options, edge, tazID, isOrigin):
    if not hasattr(options, "tazEdges"):
        # tazID -> (originEdges, destEdges)
        options.tazEdges = defaultdict(lambda: (set(), set()))
        for tazFile in options.tazFiles:
            for taz in sumolib.xml.parse(tazFile, 'taz'):
                if taz.edges:
                    edgeIDs = taz.edges.split()
                    options.tazEdges[taz.id] = (set(edgeIDs), set(edgeIDs))
                if taz.tazSource:
                    for ts in taz.tazSource:
                        options.tazEdges[taz.id][0].add(ts.id)
                if taz.tazSink:
                    for ts in taz.tazSink:
                        options.tazEdges[taz.id][1].add(ts.id)
    result = edge in options.tazEdges[tazID][0 if isOrigin else 1]
    # print(edge, tazID, isOrigin, options.tazEdges[tazID][0 if isOrigin else 1], result)
    return result


def getIntervals(options):
    begin, end, interval = parseTimeRange(options.turnFiles + options.turnRatioFiles
                                          + options.edgeDataFiles + options.odFiles)
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
    elif interval.tazRelation is None and warn:
        sys.stderr.write("Warning: No edgeRelations in interval from=%s to=%s\n" % (interval.begin, interval.end))


def parseTazCounts(interval, attr, warn):
    if interval.tazRelation is not None:
        for tazRel in interval.tazRelation:
            tazs = tuple([tazRel.attr_from] + [tazRel.to])
            value = getattr(tazRel, attr)
            yield tazs, value
    elif interval.edgeRelation is None and warn:
        sys.stderr.write("Warning: No tazRelations in interval from=%s to=%s\n" % (interval.begin, interval.end))


def parseEdgeCounts(interval, attr, warn):
    if interval.edge is not None:
        for edge in interval.edge:
            yield (edge.id,), getattr(edge, attr)
    elif warn:
        sys.stderr.write("Warning: No edges in interval from=%s to=%s\n" % (interval.begin, interval.end))


def parseDataIntervals(parseFun, fnames, begin, end, allRoutes, attr, options,
                       isOrigin=False, isDest=False, isRatio=False, isTaz=False, warn=False):
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
                        result.append(CountData(0, edges, allRoutes, isOrigin, isDest, isRatio, isTaz, options))
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


def optimize(options, countData, routes, usedRoutes, routeUsage, intervalCount, rng):
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
        # routeCounts = [0] * routes.number
        routeCountsRounded = [0] * routes.number
        for index, count in zip(relevantRoutes, routeCountsR):
            # routeCounts[index] = count
            frac, countRound = modf(count)
            if rng.random() < frac:
                countRound += 1
            routeCountsRounded[index] = int(countRound)

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
        usedRoutes.extend(sum([[i] * count for i, count in enumerate(routeCountsRounded)], []))
        # print("#usedRoutes=%s" % len(usedRoutes))
        # update countData
    else:
        print("Optimization failed")


def zero():
    # cannot pickle lambda for multiprocessing
    return 0


class Routes:
    def __init__(self, routefiles, keepStops, rng):
        self.rng = rng
        self.all = []
        self.edgeProbs = defaultdict(zero)
        self.edgeIDs = {}
        self.withProb = 0
        self.routeStops = defaultdict(list)  # list of list of stops for the given edges
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
                if keepStops and r.stop:
                    self.routeStops[edges].append(list(r.stop))

        self.unique = sorted(list(self.edgeProbs.keys()))
        self.number = len(self.unique)
        self.edges2index = dict([(e, i) for i, e in enumerate(self.unique)])
        if len(self.unique) == 0:
            print("Error: no input routes loaded", file=sys.stderr)
            sys.exit()
        self.probabilities = np.array([self.edgeProbs[e] for e in self.unique], dtype=np.float64)

    def write(self, outf, prefix, intervalPrefix, routeIndex, count, writeDist=False):
        edges = self.unique[routeIndex]
        indent = ' ' * 8
        comment = []
        probability = ""
        ID = ' id="%s%s%s"' % (prefix, intervalPrefix, routeIndex) if prefix is not None else ""
        if writeDist:
            probability = ' probability="%s"' % count
        elif ID:
            indent = ' ' * 4
            comment.append(str(count))
        if ID and edges in self.edgeIDs:
            comment.append("(%s)" % self.edgeIDs[edges])
        comment = ' '.join(comment)
        if comment:
            comment = " <!-- %s -->" % comment

        stops = []
        stopCandidates = self.routeStops.get(edges)
        if stopCandidates:
            stops = stopCandidates[self.rng.choice(range(len(stopCandidates)))]
        close = '' if stops else '/'
        outf.write('%s<route%s edges="%s"%s%s>%s\n' % (indent, ID, ' '.join(edges), probability, close, comment))
        if stops:
            for stop in stops:
                outf.write(stop.toXML(indent + ' ' * 4))
            outf.write('%s</route>\n' % indent)


def initTurnRatioSiblings(routes, countData, turnTotal):
    ratioIndices = set()
    # todo: use routes to complete incomplete sibling lists
    ratioOrigins = defaultdict(list)
    for cd in countData:
        if cd.isRatio:
            ratioOrigins[cd.edgeTuple[0]].append(cd)
            ratioIndices.add(cd.index)
            cd.origCount /= turnTotal

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
                    assert iIndex < len(options.totalCount)
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

    routes = Routes(options.routeFiles, options.keepStops, rng)

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
    ratioSummary = sumolib.miscutils.Statistics("avg interval ratio mismatch%")
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
                    if result[4][i] is not None:
                        ratioSummary.add(result[4][i], begin)
                    inputCountSummary.add(result[5][i], begin)
                    usedRoutesSummary.add(result[6][i], begin)
        else:
            for i, (begin, end) in enumerate(intervals):
                intervalPrefix = "" if len(intervals) == 1 else "%s_" % int(begin)
                intervalCount = options.totalCount[i] if options.totalCount else None
                uFlow, oFlow, gehOK, ratioPerc, inputCount, usedRoutes, _ = solveInterval(
                    options, routes, begin, end, intervalPrefix, outf, mismatchf, rng, intervalCount)
                underflowSummary.add(uFlow, begin)
                overflowSummary.add(oFlow, begin)
                gehSummary.add(gehOK, begin)
                if ratioPerc is not None:
                    ratioSummary.add(ratioPerc, begin)
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
        if ratioSummary.count() > 0:
            print(ratioSummary)


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
        uFlow, oFlow, gehOKPerc, ratioPerc, inputCount, usedRoutes, local_outf = solveInterval(
            options, routes, begin, end, intervalPrefix, local_outf, local_mismatch_outf, rng, intervalCount)

        output_list.append([begin, uFlow, oFlow, gehOKPerc, ratioPerc, inputCount, usedRoutes, local_outf.getvalue(),
                            local_mismatch_outf.getvalue() if options.mismatchOut else None])
    output_lst = list(zip(*output_list))
    return output_lst


def parseCounts(options, routes, b, e, warn=False):
    countData = (parseDataIntervals(parseTurnCounts, options.turnFiles, b, e,
                                    routes, options.turnAttr, options=options, warn=warn)
                 + parseDataIntervals(parseTurnCounts, options.turnRatioFiles, b, e,
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
    if options.tazFiles is not None:
        countData += parseDataIntervals(parseTazCounts, options.odFiles, b, e,
                                        routes, options.turnAttr, options=options,
                                        isTaz=True, warn=warn)
    for i, cd in enumerate(countData):
        cd.index = i
    return countData


def hasODCount(cdIndices, countData):
    for i in cdIndices:
        cd = countData[i]
        if cd.isTaz or (cd.isOrigin and cd.isDest):
            return True
    return False


def solveInterval(options, routes, begin, end, intervalPrefix, outf, mismatchf, rng, intervalCount):
    countData = parseCounts(options, routes, begin, end)

    ratioIndices = None
    if options.turnRatioFiles:
        ratioIndices = initTurnRatioSiblings(routes, countData, options.turnRatioTotal)

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
    if options.odFiles and not options.extraOD:
        openRoutes = [r for r in openRoutes if hasODCount(routeUsage[r], countData)]

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
                    if numSampled == 0:
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
        optimize(options, countData, routes, usedRoutes, routeUsage, intervalCount, rng)
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
                assert len(routeUsage[routeIndex]) == 0
                numSampled += 1
        else:
            print("Cannot fulfill total interval count of %s due to lack of unrestricted routes" % intervalCount,
                  file=sys.stderr)

    # avoid bias from sampling order / optimization
    rng.shuffle(usedRoutes)

    if usedRoutes:
        outf.write('<!-- begin="%s" end="%s" -->\n' % (begin, end))
        routeCounts = getRouteCounts(routes, usedRoutes)
        if options.writeRouteIDs:
            for routeIndex in sorted(set(usedRoutes)):
                routes.write(outf, options.prefix, intervalPrefix, routeIndex, routeCounts[routeIndex])
            outf.write('\n')
        elif options.writeRouteDist:
            outf.write('    <routeDistribution id="%s%s%s">\n' % (
                       options.prefix, intervalPrefix, options.writeRouteDist))
            for routeIndex in sorted(set(usedRoutes)):
                routes.write(outf, options.prefix, intervalPrefix, routeIndex,
                             routeCounts[routeIndex], writeDist=True)
            outf.write('    </routeDistribution>\n\n')

        routeID = options.writeRouteDist

        if options.writeFlows is None:
            departs = [rng.uniform(begin, end) for ri in usedRoutes]
            departs.sort()
            for i, routeIndex in enumerate(usedRoutes):
                if options.writeRouteIDs:
                    routeID = routeIndex
                vehID = options.prefix + intervalPrefix + str(i)
                depart = departs[i]
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
                        routes.write(outf, None, None, routeIndex, None)
                        outf.write('    </vehicle>\n')
        else:
            if options.writeRouteDist:
                totalCount = sum(routeCounts)
                probability = totalCount / (end - begin)
                fBegin = begin
                if options.writeFlows == "number":
                    # don't always start at the interval begin
                    fBegin += rng.uniform(0, 1 / probability)
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
                    flowID, fBegin, end, repeat,
                    options.writeRouteDist, options.vehattrs))
            else:
                # ensure flows are sorted
                flows = []
                for routeIndex in sorted(set(usedRoutes)):
                    outf2 = StringIO()
                    probability = routeCounts[routeIndex] / (end - begin)
                    fBegin = begin
                    fEnd = end
                    if options.writeFlows == "number":
                        # don't always start at the interval begin
                        fBegin += rng.uniform(0, 1 / probability)
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
                            routes.write(outf2, None, None, routeIndex, None)
                            outf2.write('    </flow>\n')
                    # secondary sort by routeIndex so we don't have to compare stringIO
                    flows.append((fBegin, routeIndex, outf2))
                flows.sort()
                for fBegin, index, outf2 in flows:
                    outf.write(outf2.getvalue())

    underflow = sumolib.miscutils.Statistics("underflow locations")
    overflow = sumolib.miscutils.Statistics("overflow locations")
    gehStats = sumolib.miscutils.Statistics("GEH")
    ratioStats = sumolib.miscutils.Statistics("turnRatio")
    numGehOK = 0.0
    gehOKPerc = 100
    ratioPerc = None
    hourFraction = (end - begin) / 3600.0
    totalCount = 0
    totalOrigCount = 0
    totalRatioCount = 0
    for cd in countData:
        if cd.isRatio:
            aProb = setPrecision("%.2f", options.precision) % cd.assignedProbability()
            cdID = "[%s] %s %s" % (' '.join(cd.edgeTuple), cd.origCount, aProb)
            ratioStats.add(cd.assignedProbability() - cd.origCount, cdID)
            totalRatioCount += cd.assignedCount
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
        cdID = "[%s] %s %s" % (' '.join(cd.edgeTuple), int(origHourly), int(localHourly))
        gehStats.add(geh, cdID)

    gehInfo = ""
    ratioInfo = ""
    if gehStats.count() > 0:
        countPercentage = "%.2f%%" % (100 * totalCount / float(totalOrigCount)) if totalOrigCount else "-"
        gehOKPerc = 100 * numGehOK / float(gehStats.count()) if countData else 100
        gehOK = "%.2f%%" % gehOKPerc if countData else "-"
        gehInfo = "total count %s (%s) at %s locations. GEH<%s for %s" % (
            totalCount, countPercentage,
            gehStats.count(),
            options.gehOk, gehOK)

    if ratioStats.count() > 0:
        if gehStats.count() > 0:
            ratioInfo = " and "
        ratioPerc = ratioStats.avg_abs() * 100
        ratioInfo += setPrecision("avg ratio mismatch %.2f%% at %s ratio locations (count %s)", options.precision) % (
            ratioPerc, ratioStats.count(), totalRatioCount)

    outputIntervalPrefix = "" if intervalPrefix == "" else "%s: " % int(begin)
    print("%sWrote %s routes (%s distinct) %s%s%s" % (
        outputIntervalPrefix,
        len(usedRoutes), len(set(usedRoutes)),
        ("achieving " if countData else "no data"),
        gehInfo, ratioInfo))

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
                if cd.isRatio:
                    deficit = setPrecision("%.2f",  options.precision) % (cd.assignedProbability() - cd.origCount)
                    mismatchf.write('        <edgeRelation from="%s" to="%s" measuredProbability="%s" deficit="%s" totalAssignedFromCount="%s"/>\n' % (  # noqa
                        cd.edgeTuple[0], cd.edgeTuple[1], cd.origCount, deficit, cd.getSiblingCount()))
                else:
                    mismatchf.write('        <edgeRelation from="%s" to="%s" measuredCount="%s" deficit="%s"/>\n' % (
                        cd.edgeTuple[0], cd.edgeTuple[1], cd.origCount, cd.count))
            else:
                print("Warning: output for edge relations with more than 2 edges not supported (%s)" % cd.edgeTuple,
                      file=sys.stderr)
        mismatchf.write('    </interval>\n')

    return sum(underflow.values), sum(overflow.values), gehOKPerc, ratioPerc, totalOrigCount, len(usedRoutes), outf


if __name__ == "__main__":
    main(get_options())
