#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    flowFromRoutes.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2007-06-28
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import math
import sys
import os

from xml.sax import make_parser, handler
from optparse import OptionParser

import detector
from detector import relError


class LaneMap:

    def get(self, key, default):
        return key[0:-2]


class DetectorRouteEmitterReader(handler.ContentHandler):

    def __init__(self, detFile):
        self._routes = {}
        self._detReader = detector.DetectorReader(detFile, LaneMap())
        self._edgeFlow = {}
        self._parser = make_parser()
        self._parser.setContentHandler(self)
        self._begin = None
        self._end = None

    def reset(self, start, end):
        self._routes = {}
        self._edgeFlow = {}
        self._begin = 60 * start
        self._end = 60 * end

    def addRouteFlow(self, route, flow):
        for edge in self._routes[route]:
            if edge not in self._edgeFlow:
                self._edgeFlow[edge] = 0
            self._edgeFlow[edge] += flow

    def startElement(self, name, attrs):
        if name == 'route':
            if 'id' in attrs:
                self._routes[attrs['id']] = attrs['edges'].split()
        if name == 'vehicle':
            if self._begin is None or float(attrs['depart']) >= self._begin:
                self.addRouteFlow(attrs['route'], 1)
        if name == 'flow':
            if 'route' in attrs:
                if self._begin is None or float(attrs['begin']) >= self._begin and float(attrs['end']) <= self._end:
                    self.addRouteFlow(attrs['route'], float(attrs['number']))
        if name == 'routeDistribution':
            if 'routes' in attrs:
                routes = attrs['routes'].split()
                nums = attrs['probabilities'].split()
                for r, n in zip(routes, nums):
                    self.addRouteFlow(r, float(n))

    def readDetFlows(self, flowFile, flowCol):
        if self._begin is None:
            return self._detReader.readFlows(flowFile, flow=flowCol)
        else:
            return self._detReader.readFlows(flowFile, flow=options.flowcol, time="Time",
                                             timeVal=self._begin / 60, timeMax=self._end / 60)

    def clearFlows(self):
        self._detReader.clearFlows()

    def calcStatistics(self):
        rSum = 0
        dSum = 0
        sumAbsDev = 0
        sumSquaredDev = 0
        sumSquaredPercent = 0
        n = 0
        for edge, detData in self._detReader._edge2DetData.iteritems():
            rFlow = self._edgeFlow.get(edge, 0)
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
        if self._begin is not None:
            print('# interval', self._begin)
        print('# avgRouteFlow avgDetFlow avgDev RMSE RMSPE')
        if n == 0:
            # avoid division by zero
            n = -1
        print('#', rSum / n, dSum / n, sumAbsDev / n,
              math.sqrt(sumSquaredDev / n), math.sqrt(sumSquaredPercent / n))

    def printFlows(self, includeDets):
        edgeIDCol = "edge " if options.edgenames else ""
        if includeDets:
            print('# detNames %sRouteFlow DetFlow ratio' % edgeIDCol)
        else:
            print('# detNames %sRouteFlow' % edgeIDCol)
        output = []
        for edge, detData in self._detReader._edge2DetData.iteritems():
            detString = []
            dFlow = []
            for group in detData:
                if group.isValid:
                    groupName = os.path.commonprefix(group.ids)
                    if groupName == "" or options.longnames:
                        groupName = ';'.join(sorted(group.ids))
                    detString.append(groupName)
                    dFlow.append(group.totalFlow)
            rFlow = len(detString) * [self._edgeFlow.get(edge, 0)]
            edges = len(detString) * [edge]
            if includeDets:
                output.extend(zip(detString, edges, rFlow, dFlow))
            else:
                output.extend(zip(detString, edges, rFlow))
        if includeDets:
            for group, edge, rflow, dflow in sorted(output):
                if dflow > 0 or options.respectzero:
                    if options.edgenames:
                        print(group, edge, rflow, dflow, relError(rflow, dflow))
                    else:
                        print(group, rflow, dflow, relError(rflow, dflow))
        else:
            for group, edge, flow in sorted(output):
                if options.edgenames:
                    print(group, edge, flow)
                else:
                    print(group, flow)


optParser = OptionParser()
optParser.add_option("-d", "--detector-file", dest="detfile",
                     help="read detectors from FILE (mandatory)", metavar="FILE")
optParser.add_option("-r", "--routes", dest="routefile",
                     help="read routes from FILE (mandatory)", metavar="FILE")
optParser.add_option("-e", "--emitters", dest="emitfile",
                     help="read emitters from FILE (mandatory)", metavar="FILE")
optParser.add_option("-f", "--detector-flow-file", dest="flowfile",
                     help="read detector flows to compare to from FILE", metavar="FILE")
optParser.add_option("-c", "--flow-column", dest="flowcol", default="qPKW",
                     help="which column contains flows", metavar="STRING")
optParser.add_option("-z", "--respect-zero", action="store_true", dest="respectzero",
                     default=False, help="respect detectors without data (or with permanent zero) with zero flow")
optParser.add_option("-D", "--dfrouter-style", action="store_true", dest="dfrstyle",
                     default=False, help="emitter files in dfrouter style (explicit routes)")
optParser.add_option("-i", "--interval", type="int", help="aggregation interval in minutes")
optParser.add_option("--long-names", action="store_true", dest="longnames",
                     default=False, help="do not use abbreviated names for detector groups")
optParser.add_option("--edge-names", action="store_true", dest="edgenames",
                     default=False, help="include detector group edge name in output")
optParser.add_option( "-b", "--begin", type="float", default=0, help="begin time in minutes")
optParser.add_option( "--end", type="float", default=None, help="end time in minutes")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.detfile or not options.routefile or not options.emitfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()
if options.verbose:
    print("Reading detectors")
reader = DetectorRouteEmitterReader(options.detfile)
parser.setContentHandler(reader)
if options.interval:
    haveFlows = True
    start = options.begin # minutes
    while ((options.end is None and haveFlows)
            or (options.end is not None and start < options.end)):
        end = start + options.interval
        if options.end is not None:
            end = min(end, options.end)
        reader.reset(start, end)
        if options.verbose:
            print("Reading routes")
        parser.parse(options.routefile)
        if options.verbose:
            print("Reading emitters")
        parser.parse(options.emitfile)
        if options.flowfile:
            if options.verbose:
                print("Reading flows")
            haveFlows = reader.readDetFlows(options.flowfile, options.flowcol)
        if haveFlows:
            reader.printFlows(bool(options.flowfile))
            if options.flowfile:
                reader.calcStatistics()
            reader.clearFlows()
        start += options.interval
else:
    if options.verbose:
        print("Reading routes")
    parser.parse(options.routefile)
    if options.verbose:
        print("Reading emitters")
    parser.parse(options.emitfile)
    if options.flowfile:
        if options.verbose:
            print("Reading flows")
        reader.readDetFlows(options.flowfile, options.flowcol)
    reader.printFlows(bool(options.flowfile))
    if options.flowfile:
        reader.calcStatistics()
