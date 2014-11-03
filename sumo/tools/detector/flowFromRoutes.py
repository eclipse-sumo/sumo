#!/usr/bin/env python
"""
@file    flowFromRoutes.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2007-06-28
@version $Id$

This script recreates a flow file from routes and emitters.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2007-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import math, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

import detector

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
        
    def addEdgeFlow(self, edge, flow):
        if not edge in self._edgeFlow:
            self._edgeFlow[edge] = 0
        self._edgeFlow[edge] += flow

    def startElement(self, name, attrs):
        if name == 'route':
            if attrs.has_key('id'):
                self._routes[attrs['id']] = attrs['edges'].split()
        if name == 'vehicle':
            for edge in self._routes[attrs['route']]:
                self.addEdgeFlow(edge, 1)

    def readDetFlows(self, flowFile):
        self._detReader.readFlows(flowFile)

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
        print '# avgRouteFlow avgDetFlow avgDev RMSE RMSPE'
        print '#', rSum/n, dSum/n, sumAbsDev/n, math.sqrt(sumSquaredDev/n), math.sqrt(sumSquaredPercent/n)

    def printFlows(self, includeDets):
        if includeDets:
            print '# detNames RouteFlow DetFlow'
        else:
            print '# detNames RouteFlow'
        output = []
        for edge, detData in self._detReader._edge2DetData.iteritems():
            detString = []
            dFlow = []
            for group in detData:
                if group.isValid:
                    detString.append(string.join(sorted(group.ids), ';'))
                    dFlow.append(group.totalFlow)
            rFlow = len(detString) * [self._edgeFlow.get(edge, 0)]
            if includeDets:
                output.extend(zip(detString, rFlow, dFlow))
            else:
                output.extend(zip(detString, rFlow))
        if includeDets:
            for group, rflow, dflow in sorted(output):
                if dflow > 0 or options.respectzero:
                    print group, rflow, dflow
        else:
            for group, flow in sorted(output):
                print group, flow


optParser = OptionParser()
optParser.add_option("-d", "--detector-file", dest="detfile",
                     help="read detectors from FILE (mandatory)", metavar="FILE")
optParser.add_option("-r", "--routes", dest="routefile",
                     help="read routes from FILE (mandatory)", metavar="FILE")
optParser.add_option("-e", "--emitters", dest="emitfile",
                     help="read emitters from FILE (mandatory)", metavar="FILE")
optParser.add_option("-f", "--detector-flow-file", dest="flowfile",
                     help="read detector flows to compare to from FILE", metavar="FILE")
optParser.add_option("-z", "--respect-zero", action="store_true", dest="respectzero",
                     default=False, help="respect detectors without data (or with permanent zero) with zero flow")
optParser.add_option("-D", "--dfrouter-style", action="store_true", dest="dfrstyle",
                     default=False, help="emitter files in dfrouter style (explicit routes)")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.detfile or not options.routefile or not options.emitfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()
if options.verbose:
    print "Reading detectors"
reader = DetectorRouteEmitterReader(options.detfile)
parser.setContentHandler(reader)
if options.verbose:
    print "Reading routes"
parser.parse(options.routefile)
if options.verbose:
    print "Reading emitters"
parser.parse(options.emitfile)
if options.flowfile:
    reader.readDetFlows(options.flowfile)
reader.printFlows(bool(options.flowfile))
if options.flowfile:
    reader.calcStatistics()
