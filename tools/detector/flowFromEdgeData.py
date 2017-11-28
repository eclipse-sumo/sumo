#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    flowFromRoutes.py
# @author  Jakob Erdmann
# @date    2017-11-27
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import math
import sys
import os

from xml.sax import make_parser, handler
from optparse import OptionParser
from collections import defaultdict

import detector
from detector import relError

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.xml import parse


def get_options(args=None):
    optParser = OptionParser()
    optParser.add_option("-d", "--detector-file", dest="detfile",
                         help="read detectors from FILE (mandatory)", metavar="FILE")
    optParser.add_option("-e", "--edgedata-file", dest="edgeDataFile",
                         help="read edgeData from FILE (mandatory)", metavar="FILE")
    optParser.add_option("-f", "--detector-flow-file", dest="flowfile",
                         help="read detector flows to compare to from FILE (mandatory)", metavar="FILE")
    optParser.add_option("-c", "--flow-column", dest="flowcol", default="qPKW",
                         help="which column contains flows", metavar="STRING")
    optParser.add_option("-z", "--respect-zero", action="store_true", dest="respectzero",
                         default=False, help="respect detectors without data (or with permanent zero) with zero flow")
    optParser.add_option("-i", "--interval", type="int", default="1440", help="aggregation interval in minutes")
    optParser.add_option("--long-names", action="store_true", dest="longnames",
                         default=False, help="do not use abbreviated names for detector groups")
    optParser.add_option("--edge-names", action="store_true", dest="edgenames",
                         default=False, help="include detector group edge name in output")
    optParser.add_option( "-b", "--begin", type="float", default=0, help="begin time in minutes")
    optParser.add_option( "--end", type="float", default=1440, help="end time in minutes (default 1440)")
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args(args=args)
    if not options.detfile or not options.edgeDataFile or not options.flowfile:
        optParser.print_help()
        sys.exit()

    options.beginS = options.begin * 60
    options.endS = options.end * 60
    return options


def readEdgeData(edgeDataFile, begin, end):
    edgeFlow = defaultdict(lambda : 0)
    for interval in parse(edgeDataFile, "interval", attr_conversions={"begin":float, "end":float}):
        interval.begin
        #print("reading intervals for begin=%s end=%s (current interval begin=%s end=%s)" % (begin, end, interval.begin, interval.end))
        if interval.begin < end and interval.end > begin:
            # if read interval is partly outside comparison interval we must scale demand
            validInterval = interval.end - interval.begin
            if interval.begin < begin:
                validInterval -= begin - interval.begin
            if interval.end > end:
                validInterval -= interval.end - end
            scale = validInterval / (interval.end - interval.begin)
            # store data
            for edge in interval.edge:
                edgeFlow[edge.id] += (int(edge.departed) + int(edge.entered)) * scale
    return edgeFlow


def printFlows(options, edgeFlow, detReader):
    edgeIDCol = "edge " if options.edgenames else ""
    print('# detNames %sRouteFlow DetFlow ratio' % edgeIDCol)
    output = []
    for edge, detData in detReader._edge2DetData.iteritems():
        detString = []
        dFlow = []
        for group in detData:
            if group.isValid:
                groupName = os.path.commonprefix(group.ids)
                if groupName == "" or options.longnames:
                    groupName = ';'.join(sorted(group.ids))
                detString.append(groupName)
                dFlow.append(group.totalFlow)
        rFlow = len(detString) * [edgeFlow.get(edge, 0)]
        edges = len(detString) * [edge]
        output.extend(zip(detString, edges, rFlow, dFlow))
    for group, edge, rflow, dflow in sorted(output):
        if dflow > 0 or options.respectzero:
            if options.edgenames:
                print(group, edge, rflow, dflow, relError(rflow, dflow))
            else:
                print(group, rflow, dflow, relError(rflow, dflow))

def calcStatistics(options, begin, edgeFlow, detReader):
    rSum = 0
    dSum = 0
    sumAbsDev = 0
    sumSquaredDev = 0
    sumSquaredPercent = 0
    n = 0
    for edge, detData in detReader._edge2DetData.iteritems():
        rFlow = edgeFlow.get(edge, 0)
        for group in detData:
            if group.isValid:
                dFlow = group.totalFlow
                if dFlow > 0 or options.respectzero:
                    rSum += rFlow
                    dSum += dFlow
                    dev = float(abs(rFlow - dFlow))
                    sumAbsDev += dev
                    sumSquaredDev += dev * dev
                    if dFlow > 0:
                        sumSquaredPercent += dev * dev / dFlow / dFlow
                    n += 1
    print('# interval', begin)
    print('# avgRouteFlow avgDetFlow avgDev RMSE RMSPE')
    if n == 0:
        # avoid division by zero
        n = -1
    print('#', rSum / n, dSum / n, sumAbsDev / n,
          math.sqrt(sumSquaredDev / n), math.sqrt(sumSquaredPercent / n))

class LaneMap:
    def get(self, key, default):
        return key[0:-2]


def main(options):
    detReader = detector.DetectorReader(options.detfile, LaneMap())
    time = options.beginS
    while time < options.endS:
        intervalBeginM = time / 60
        intervalEndM = intervalBeginM + options.interval
        if options.verbose:
            print("Reading flows")
        detReader.readFlows(options.flowfile, flow=options.flowcol, time="Time", timeVal=intervalBeginM, timeMax=intervalEndM)
        if options.verbose:
            print("Reading edgeData")
        edgeFlow = readEdgeData(options.edgeDataFile, time, time + options.interval * 60)
        printFlows(options, edgeFlow, detReader)
        calcStatistics(options, intervalBeginM, edgeFlow, detReader)
        detReader.clearFlows()
        time += options.interval * 60

if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
