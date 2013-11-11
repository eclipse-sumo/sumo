#!/usr/bin/env python
"""
@file    flowrouter.py
@author  Michael Behrisch
@date    2007-06-28
@version $Id$

This script does flow routing similar to the dfrouter.
It has three mandatory parameters, the SUMO net (.net.xml), a file
specifying detectors and one for the flows. It may detect the type
of the detectors (source, sink, inbetween) itself or read it from
the detectors file.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2007-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, random, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.output

import detector

# Vertex class which stores incoming and outgoing edges as well as
# auxiliary data for the flow computation. The members are accessed
# directly.
class Vertex:

    def __init__(self):
        self.inEdges = []
        self.outEdges = []
        self.reset()

    def reset(self):
        self.inPathEdge = None
        self.flowDelta = sys.maxint
        self.gain = 0

    def update(self, edge, flow, isForward):
        self.inPathEdge = edge
        self.flowDelta = flow
        if isForward:
            numSatEdges = edge.source.gain / edge.source.flowDelta
            self.gain = numSatEdges * flow
            if edge.capacity < sys.maxint:
                self.gain += flow
        else:
            numSatEdges = edge.target.gain / edge.target.flowDelta
            self.gain = numSatEdges * flow
            if edge.capacity < sys.maxint:
                self.gain -= flow

    def __repr__(self):
        return "<%s,%s,%s>" % (self.inPathEdge, self.flowDelta, self.gain)


# Edge class which stores start and end vertex, type amd label of the edge
# as well as flow and capacity for the flow computation and some parameters
# read from the net. The members are accessed directly.
class Edge:

    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.kind = kind
        self.maxSpeed = 0.0
        self.length = 0.0
        self.detGroup = []
        self.reset()
    
    def reset(self):
        self.capacity = sys.maxint
        self.startCapacity = sys.maxint
        self.flow = 0
        self.routes = []

    def __repr__(self):
        cap = str(self.capacity)
        if self.capacity == sys.maxint:
            cap = "inf"
        return self.kind+"_"+self.label+"<"+str(self.flow)+"|"+cap+">"


# Route class storing the list of edges and the frequency of a route.
class Route:

    def __init__(self, freq, edges):
        self.frequency = freq
        self.edges = edges

    def __repr__(self):
        result = str(self.frequency) + " * ["
        lastLabel = ""
        for edge in self.edges:
            if edge.kind == "real":
                if lastLabel:
                    result += lastLabel + ", "
                lastLabel = edge.label
        return result + lastLabel + "]"


# Net class which stores the network (vertex and edge collection) and the
# routes. All the algorithmic stuff and the output generation are also
# inside this class. The members are either "private" or have get/add/remove
# methods.
class Net:

    def __init__(self):
        self._vertices = []
        self._edges = {}
        self._internalEdges = []
        self._possibleSources = set()
        self._possibleSinks = set()
        self._source = self.newVertex()
        self._sink = self.newVertex()

    def newVertex(self):
        v = Vertex()
        self._vertices.append(v)
        return v

    def getEdge(self, edgeLabel):
        return self._edges[edgeLabel]

    def addEdge(self, edgeObj):
        edgeObj.source.outEdges.append(edgeObj)
        edgeObj.target.inEdges.append(edgeObj)
        if edgeObj.kind == "real":
            self._edges[edgeObj.label] = edgeObj
        else:
            self._internalEdges.append(edgeObj)

    def removeEdge(self, edgeObj):
        edgeObj.source.outEdges.remove(edgeObj)
        edgeObj.target.inEdges.remove(edgeObj)
        if edgeObj.kind == "real":
            del self._edges[edgeObj.label]
            checkEdges = edgeObj.source.inEdges.union(edgeObj.target.outEdges)
            for edge in checkEdges:
                if edge.kind != "real":
                    self.removeEdge(edge)

    def addIsolatedRealEdge(self, edgeLabel):
        self.addEdge(Edge(edgeLabel, self.newVertex(), self.newVertex(),
                          "real"))

    def addSourceEdge(self, edgeObj):
        newEdge = Edge("s_"+edgeObj.label, self._source, edgeObj.source,
                       "source")
        self.addEdge(newEdge)

    def addSinkEdge(self, edgeObj):
        newEdge = Edge("t_"+edgeObj.label, edgeObj.target, self._sink,
                       "sink")
        self.addEdge(newEdge)

    def trimNet(self):
        if options.minspeed > 0.0:
            if options.verbose:
                print "Removing edges with maxspeed < %s," % options.minspeed,
            # The code in the following loop assumes there are still all
            # auxiliary junction edges present.
            for edgeObj in self._edges.values():
                if edgeObj.maxSpeed < options.minspeed:
                    if len(edgeObj.detGroup) == 0 or not options.keepdet:
                        for auxpred in edgeObj.source.inEdges:
                            for realpred in auxpred.source.inEdges:
                                if realpred.maxSpeed >= options.minspeed:
                                    self._possibleSinks.add(realpred)
                        for auxsucc in edgeObj.target.outEdges:
                            for realsucc in auxsucc.target.outEdges:
                                if realsucc.maxSpeed >= options.minspeed:
                                    self._possibleSources.add(realsucc)
                        self.removeEdge(edgeObj)
            if options.verbose:
                print len(self._edges), "left"
            if options.trimfile:
                trimOut = open(options.trimfile, 'w')
                for edge in self._edges.values():
                    print >> trimOut, "edge:"+edge.label
                trimOut.close()

    def detectSourceSink(self, sources, sinks):
        self.trimNet()
        for id in sources:
            self.addSourceEdge(self.getEdge(id))
        for id in sinks:
            self.addSinkEdge(self.getEdge(id))
        for edgeObj in self._edges.itervalues():
            if len(sources) == 0 and (len(edgeObj.source.inEdges) == 0 or edgeObj in self._possibleSources):
                self.addSourceEdge(edgeObj)
            if len(sinks) == 0 and (len(edgeObj.target.outEdges) == 0 or edgeObj in self._possibleSinks):
                self.addSinkEdge(edgeObj)
        if len(self._sink.inEdges) == 0:
            print "Error! No sinks found."
            return False
        if len(self._source.outEdges) == 0:
            print "Error! No sources found."
            return False
        return True

    def initNet(self):
        for edge in self._internalEdges:
            edge.reset()
        for edge in self._edges.itervalues():
            edge.reset()
            if len(edge.detGroup) > 0:
                edge.capacity = 0
                for group in edge.detGroup:
                    if int(group.totalFlow) > edge.capacity:
                        edge.capacity = int(group.totalFlow)
            if not options.respectzero and edge.capacity == 0:
                edge.capacity = sys.maxint
            edge.startCapacity = edge.capacity
        if options.verbose:
            unlimitedSource = 0
            for edgeObj in self._source.outEdges:
                for src in edgeObj.target.outEdges:
                    if src.capacity == sys.maxint:
                        unlimitedSource += 1
            unlimitedSink = 0
            for edgeObj in self._sink.inEdges:
                for sink in edgeObj.source.inEdges:
                    if sink.capacity == sys.maxint:
                        unlimitedSink += 1
            print len(self._source.outEdges), "sources,",
            print unlimitedSource, "unlimited"
            print len(self._sink.inEdges), "sinks,", unlimitedSink, "unlimited"

    def splitRoutes(self, stubs, currEdge):
        newStubs = []
        while len(stubs) > 0:
            routeStub = stubs.pop()
            if len(routeStub.edges) > 0 and currEdge == routeStub.edges[0]:
                routeStub.edges.pop(0)
                newStubs.append(routeStub)
            else:
                while routeStub.frequency > 0:
                    route = currEdge.routes[0]
                    edgePos = route.edges.index(currEdge)
                    newRoute = Route(min(routeStub.frequency, route.frequency),
                                     route.edges[:edgePos] + routeStub.edges)
                    for edge in newRoute.edges:
                        edge.routes.append(newRoute)
                    newStubs.append(Route(newRoute.frequency,
                                          route.edges[edgePos+1:]))
                    route.frequency -= newRoute.frequency
                    if route.frequency == 0:
                        for edge in route.edges:
                            edge.routes.remove(route)
                    routeStub.frequency -= newRoute.frequency
        stubs.extend(newStubs)

    def updateFlow(self, startVertex, endVertex):
        assert endVertex.flowDelta < sys.maxint
        cycleStartStep = (startVertex == endVertex)
        currVertex = endVertex
        stubs = [Route(endVertex.flowDelta, [])]
        while currVertex != startVertex or cycleStartStep:
            cycleStartStep = False
            currEdge = currVertex.inPathEdge
            if currEdge.target == currVertex:
                currEdge.flow += endVertex.flowDelta
                currVertex = currEdge.source
                for routeStub in stubs:
                    routeStub.edges.insert(0, currEdge)
            else:
                currEdge.flow -= endVertex.flowDelta
                currVertex = currEdge.target
                self.splitRoutes(stubs, currEdge)
        for route in stubs:
            for edge in route.edges:
                edge.routes.append(route)

    def findPath(self, startVertex, pathStart):
        queue = [startVertex]
        while len(queue) > 0:
            currVertex = queue.pop(0)
            if currVertex == self._sink or (currVertex == self._source and currVertex.inPathEdge):
                self.updateFlow(pathStart, currVertex)
                return True
            for edge in currVertex.outEdges:
                if not edge.target.inPathEdge and edge.flow < edge.capacity:
                    if edge.target != self._sink or currVertex.gain > 0:
                        queue.append(edge.target)
                        edge.target.update(edge, min(currVertex.flowDelta,
                                                     edge.capacity - edge.flow),
                                           True)
            for edge in currVertex.inEdges:
                if not edge.source.inPathEdge and edge.flow > 0:
                    if edge.source != self._source or currVertex.gain > 0:
                        queue.append(edge.source)
                        edge.source.update(edge, min(currVertex.flowDelta,
                                                     edge.flow), False)
        return False

    def savePulledPath(self, startVertex, unsatEdge, pred):
        numSatEdges = 1
        currVertex = startVertex
        while currVertex != unsatEdge.source:
            currEdge = pred[currVertex]
            if currEdge.target == currVertex:
                currEdge.source.inPathEdge = currEdge
                currVertex = currEdge.source
                if currEdge.capacity < sys.maxint:
                    numSatEdges -= 1
            else:
                currEdge.target.inPathEdge = currEdge
                currVertex = currEdge.target
                if currEdge.capacity < sys.maxint:
                    numSatEdges += 1
        startVertex.inPathEdge = None
        unsatEdge.target.flowDelta = startVertex.flowDelta
        unsatEdge.target.gain = startVertex.flowDelta * numSatEdges

    def pullFlow(self, unsatEdge):
        if options.verbose:
            print "Trying to increase flow on", unsatEdge
        for vertex in self._vertices:
            vertex.reset()
        pred = {unsatEdge.target:unsatEdge, unsatEdge.source:unsatEdge}
        unsatEdge.target.inPathEdge = unsatEdge
        unsatEdge.source.flowDelta = unsatEdge.capacity - unsatEdge.flow
        queue = [unsatEdge.source]
        while len(queue) > 0:
            currVertex = queue.pop(0)
            if currVertex == self._source or currVertex == self._sink:
                self.savePulledPath(currVertex, unsatEdge, pred)
                return self.findPath(unsatEdge.target, currVertex)
            for edge in currVertex.inEdges:
                if edge.source not in pred and edge.flow < edge.capacity:
                    queue.append(edge.source)
                    pred[edge.source] = edge
                    edge.source.flowDelta = min(currVertex.flowDelta, edge.capacity - edge.flow)
            for edge in currVertex.outEdges:
                if edge.target not in pred and edge.flow > 0:
                    queue.append(edge.target)
                    pred[edge.target] = edge
                    edge.target.flowDelta = min(currVertex.flowDelta, edge.flow)
        return False

    def calcRoutes(self):
        self.initNet()
        pathFound = True
        while pathFound:
            for vertex in self._vertices:
                vertex.reset()
            pathFound = self.findPath(self._source, self._source)
            if not pathFound:
                for edge in self._edges.itervalues():
                    if edge.capacity < sys.maxint:
                        while edge.flow < edge.capacity and self.pullFlow(edge):
                            pathFound = True
        # the rest of this function only tests assertions
        for vertex in self._vertices:
            sum = 0
            for preEdge in vertex.inEdges:
                sum += preEdge.flow
            for succEdge in vertex.outEdges:
                sum -= succEdge.flow
                flowSum = 0
                for route in succEdge.routes:
                    assert route.frequency > 0
                    flowSum += route.frequency
                assert flowSum == succEdge.flow
            assert vertex == self._source or vertex == self._sink or sum == 0

    def writeRoutes(self, routeOut, suffix=""):
        for edge in self._source.outEdges:
            routeByEdges = {}
            for route in edge.routes:
                key = tuple([e.label for e in route.edges if e.kind == "real"])
                if key in routeByEdges:
                    routeByEdges[key].frequency += route.frequency
                else:
                    routeByEdges[key] = route
            edge.routes = routeByEdges.values()
            for id, route in enumerate(edge.routes):
                firstReal = ''
                lastReal = None
                routeString = ''
                for redge in route.edges:
                    if redge.kind == "real":
                        if options.lanebased:
                            routeString += redge.label[:redge.label.rfind("_")] + " "
                        else:
                            routeString += redge.label + " "
                        if firstReal == '':
                            firstReal = redge.label
                        lastReal = redge
                assert firstReal != '' and lastReal != None
                routeID = "%s.%s%s" % (firstReal, id, suffix)
                print >> routeOut, '    <route id="%s" edges="%s"/>' % (routeID, routeString.strip())

    def writeEmitters(self, emitOut, begin=0, end=3600, suffix=""):
        if not emitOut:
            return
        for srcEdge in self._source.outEdges:
            if len(srcEdge.routes) == 0:
                continue
            assert len(srcEdge.target.outEdges) == 1
            edge = srcEdge.target.outEdges[0]
            vtype = ' type="%s"' % options.vtype if options.vtype else ""
            if len(srcEdge.routes) == 1:
                print >> emitOut, '    <flow id="src_%s%s"%s route="%s.0%s" number="%s" begin="%s" end="%s"/>' % (edge.label, suffix, vtype, edge.label, suffix, srcEdge.flow, begin, end)
            else:
                ids = " ".join(["%s.%s%s" % (edge.label, id, suffix) for id in range(len(srcEdge.routes))])
                probs = " ".join([str(route.frequency) for route in srcEdge.routes])
                print >> emitOut, '    <flow id="src_%s%s"%s number="%s" begin="%s" end="%s">' % (edge.label, suffix, vtype, srcEdge.flow, begin, end)
                print >> emitOut, '        <routeDistribution routes="%s" probabilities="%s"/>' % (ids, probs)
                print >> emitOut, '    </flow>'

    def writeFlowPOIs(self, poiOut, suffix=""):
        if not poiOut:
            return
        for edge in self._edges.itervalues():
            color = "0,0,1"
            for src in edge.source.inEdges:
                if src.source == self._source:
                    color = "0,1,0"
                    break
            for sink in edge.target.outEdges:
                if sink.target == self._sink:
                    color = "1,"+color[2]+",0"
                    break
            label = edge.label
            flow = str(edge.flow)
            cap = str(edge.startCapacity)
            if edge.startCapacity == sys.maxint:
                cap = "inf"
            print >> poiOut, '    <poi id="' + label + '_f' + flow + 'c' + cap + suffix + '"',
            print >> poiOut, 'color = "' + color + '" lane="' + label + '_0"',
            print >> poiOut, ' pos="' + str(random.random()*edge.length) + '"/>'


# The class for parsing the XML and CSV input files. The data parsed is
# written into the net. All members are "private".
class NetDetectorFlowReader(handler.ContentHandler):

    def __init__(self, net):
        self._net = net
        self._edge = ''
        self._lane2edge = {}
        self._detReader = None

    def startElement(self, name, attrs):
        if name == 'edge' and (not attrs.has_key('function') or attrs['function'] != 'internal'):
            self._edge = attrs['id']
            if not options.lanebased:
                self._net.addIsolatedRealEdge(attrs['id'])
        elif name == 'connection':
            fromEdgeID = attrs['from']
            if fromEdgeID[0] != ":":
                toEdgeID = attrs['to']
                if options.lanebased:
                    fromEdgeID += "_" + attrs["fromLane"]
                    toEdgeID += "_" + attrs["toLane"]
                newEdge = Edge(fromEdgeID+"_"+toEdgeID, self._net.getEdge(fromEdgeID).target,
                               self._net.getEdge(toEdgeID).source)
                self._net.addEdge(newEdge)
        elif name == 'lane' and self._edge != '':
            if options.lanebased:
                self._net.addIsolatedRealEdge(attrs['id'])
                self._edge = attrs['id']
            self._lane2edge[attrs['id']] = self._edge
            edgeObj = self._net.getEdge(self._edge)
            edgeObj.maxSpeed = max(edgeObj.maxSpeed, float(attrs['speed']))
            edgeObj.length = float(attrs['length'])

    def endElement(self, name):
        if name == 'edge':
            self._edge = ''

    def readDetectors(self, detFile):
        self._detReader = detector.DetectorReader(detFile, self._lane2edge)
        for edge, detGroups in self._detReader._edge2DetData.iteritems():
            for group in detGroups:
                if group.isValid:
                    self._net.getEdge(edge).detGroup.append(group)
        sources = set()
        sinks = set()
        for det in sumolib.output.parse(detFile, "detectorDefinition"):
            if hasattr(det, "type"):
                if det.type == "source":
                    if options.lanebased:
                        sources.add(det.lane)
                    else:
                        sources.add(det.lane[:det.lane.rfind("_")])
                if det.type == "sink":
                    if options.lanebased:
                        sinks.add(det.lane)
                    else:
                        sinks.add(det.lane[:det.lane.rfind("_")])
        return sources, sinks

    def readFlows(self, flowFile, t=None):
        if t is None:
            return self._detReader.readFlows(flowFile, flow=options.flowcol)
        else:
            return self._detReader.readFlows(flowFile, flow=options.flowcol, time="Time", timeVal=t)            

    def clearFlows(self):
        self._detReader.clearFlows()


def warn(msg):
    if not options.quiet:
        print >> sys.stderr, msg

def addFlowFile(option, opt_str, value, parser):
    if not getattr(parser.values, option.dest, None):
        setattr(parser.values, option.dest, [])
    fileList = getattr(parser.values, option.dest)
    fileList.append(value)
    index = 0
    while index < len(parser.rargs) and not parser.rargs[index].startswith("-"):
        index += 1
    fileList.extend(parser.rargs[0:index])
    parser.rargs = parser.rargs[index:]

optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-d", "--detector-file", dest="detfile",
                     help="read detectors from FILE (mandatory)", metavar="FILE")
optParser.add_option("-f", "--detector-flow-files", dest="flowfiles",
                     action="callback", callback=addFlowFile, type="string",
                     help="read detector flows from FILE(s) (mandatory)", metavar="FILE")
optParser.add_option("-c", "--flow-column", dest="flowcol", default="qPKW",
                     help="which column contains flows", metavar="STRING")
optParser.add_option("-o", "--routes-output", dest="routefile",
                     help="write routes to FILE", metavar="FILE")
optParser.add_option("-e", "--emitters-output", dest="emitfile",
                     help="write emitters to FILE and create files per emitter (needs -o)", metavar="FILE")
optParser.add_option("-y", "--vtype", help="vType to use", metavar="STRING")
optParser.add_option("-t", "--trimmed-output", dest="trimfile",
                     help="write edges of trimmed network to FILE", metavar="FILE")
optParser.add_option("-p", "--flow-poi-output", dest="flowpoifile",
                     help="write resulting flows as SUMO POIs to FILE", metavar="FILE")
optParser.add_option("-m", "--min-speed", type="float", dest="minspeed",
                     default=0.0, help="only consider edges where the fastest lane allows at least this maxspeed (m/s)")
optParser.add_option("-D", "--keep-det", action="store_true", dest="keepdet",
                     default=False, help='keep edges with detectors when deleting "slow" edges')
optParser.add_option("-z", "--respect-zero", action="store_true", dest="respectzero",
                     default=False, help="respect detectors without data (or with permanent zero) with zero flow")
optParser.add_option("-l", "--lane-based", action="store_true", dest="lanebased",
                     default=False, help="do not aggregate detector data and connections to edges")
optParser.add_option("-i", "--interval", type="int", help="aggregation interval in minutes")
optParser.add_option("-q", "--quiet", action="store_true", dest="quiet",
                     default=False, help="suppress warnings")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.netfile or not options.detfile or not options.flowfiles:
    optParser.print_help()
    sys.exit()
if options.emitfile and not options.routefile:
    optParser.print_help()
    sys.exit()
parser = make_parser()
if options.verbose:
    print "Reading net"
net = Net()
reader = NetDetectorFlowReader(net)
parser.setContentHandler(reader)
parser.parse(options.netfile)
if options.verbose:
    print len(net._edges), "edges read"
    print "Reading detectors"
sources, sinks = reader.readDetectors(options.detfile)
if net.detectSourceSink(sources, sinks):
    routeOut = None
    if options.routefile:
        routeOut = open(options.routefile, 'w')
        print >> routeOut, "<routes>"
    emitOut = None
    if options.emitfile:
        emitOut = open(options.emitfile, 'w')
        print >> emitOut, "<additional>"
    poiOut = None
    if options.flowpoifile:
        poiOut = open(options.flowpoifile, 'w')
        print >> poiOut, "<pois>"
    if options.interval:
        haveFlows = True
        start = 0
        while haveFlows:
            suffix = ".%s" % start
            if options.verbose:
                print "Reading flows"
            for flow in options.flowfiles:
                haveFlows = reader.readFlows(flow, start)
            if haveFlows:
                if options.verbose:
                    print "Calculating routes"
                net.calcRoutes()
                if routeOut:
                    net.writeRoutes(routeOut, suffix)
                else:
                    for edge in net._source.outEdges:
                        for route in edge.routes:
                            print route
                net.writeEmitters(emitOut, 60*start, 60*(start+options.interval), suffix)
                net.writeFlowPOIs(poiOut, suffix)
            reader.clearFlows()
            start += options.interval
    else:
        if options.verbose:
            print "Reading flows"
        for flow in options.flowfiles:
            reader.readFlows(flow)
        if options.verbose:
            print "Calculating routes"
        net.calcRoutes()
        if routeOut:
            net.writeRoutes(routeOut)
        else:
            for edge in net._source.outEdges:
                for route in edge.routes:
                    print route
        net.writeEmitters(emitOut)
        net.writeFlowPOIs(poiOut)
    if routeOut:
        print >> routeOut, "</routes>"
        routeOut.close()
    if emitOut:
        print >> emitOut, "</additional>"
        emitOut.close()
    if poiOut:
        print >> poiOut, "</pois>"
        poiOut.close()
