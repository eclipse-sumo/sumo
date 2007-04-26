#!/usr/bin/python
# This script does flow routing similar to the dfrouter.
# It has three mandatory parameters, the SUMO net (.net.xml), a file
# specifying detectors and one for the flows. It may detect the type
# of the detectors (source, sink, inbetween) itself or read it from
# the detectors file.
import os, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

MAX_POS_DEVIATION = 1

class Vertex:

    def __init__(self):
        self.inEdges = set()
        self.outEdges = set()


class Edge:

    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.capacity = sys.maxint
        self.startCapacity = sys.maxint
        self.flow = 0
        self.kind = kind
        self.maxSpeed = 0.0
        self.laneCount = 0
        self.finalizer = None
        self.detPos = []
        self.detGroup = []
        self.detFlow = []

    def addDet(self, pos, det):
        for index, compPos in enumerate(self.detPos):
            if abs(compPos - pos) <= MAX_POS_DEVIATION:
                self.detGroup[index].append(det)
                return
        self.detPos.append(pos)
        self.detGroup.append([det])
        self.detFlow.append(0)

    def addFlow(self, det, flow):
        for index, group in enumerate(self.detGroup):
            if det in group:
                self.detFlow[index] += flow
                return
        assert False

    def __repr__(self):
        cap = str(self.capacity)
        if self.capacity == sys.maxint:
            cap = "inf"
        return self.kind+"_"+self.label+"<"+str(self.flow)+"|"+cap+">"


class Net:

    def __init__(self):
        self._vertices = []
        self._edges = {}
        self._source = self.newVertex()
        self._sink = self.newVertex()
        self._routeOut = None
        self._routes = {}
        self._routeFreq = {}

    def newVertex(self):
        v = Vertex()
        self._vertices.append(v)
        return v

    def getEdge(self, edgeLabel):
        return self._edges[edgeLabel]

    def addEdge(self, edgeObj):
        edgeObj.source.outEdges.add(edgeObj)
        edgeObj.target.inEdges.add(edgeObj)
        if edgeObj.kind == "real":
            self._edges[edgeObj.label] = edgeObj

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
        self.addEdge(Edge(edgeLabel, self.newVertex(), self.newVertex(), "real"))

    def addSourceEdge(self, edgeObj):
        newEdge = Edge("s_"+edgeObj.label, self._source, edgeObj.source, "source")
        self.addEdge(newEdge)

    def addSinkEdge(self, edgeObj):
        newEdge = Edge("t_"+edgeObj.label, edgeObj.target, self._sink, "sink")
        self.addEdge(newEdge)

    def trimNet(self):
        if options.minspeed > 0.0:
            if options.verbose:
                print "Removing edges with maxspeed <", options.minspeed, "... ", 
            for edgeObj in self._edges.values():
                if edgeObj.maxSpeed < options.minspeed:
                    delete = True
                    for auxpred in edgeObj.source.inEdges:
                        for realpred in auxpred.source.inEdges:
                            if realpred.maxSpeed >= options.minspeed:
                                delete = False
                                break
                        if not delete: break
                    for auxsucc in edgeObj.target.outEdges:
                        for realsucc in auxsucc.target.outEdges:
                            if realsucc.maxSpeed >= options.minspeed:
                                delete = False
                                break
                        if not delete: break
                    if options.keepdet and edgeObj.capacity < sys.maxint:
                        delete = False
                    if delete:
                        self.removeEdge(edgeObj)
            if options.verbose:
                print len(self._edges), "left"
            if options.trimfile:
                trimOut = open(options.trimfile, 'w')
                for edge in self._edges.values():
                    print >> trimOut, "edge:"+edge.label
                trimOut.close()

    def detectSourceSink(self):
        for edgeObj in self._edges.itervalues():
            if len(edgeObj.source.inEdges) == 0:
                self.addSourceEdge(edgeObj)
            if len(edgeObj.target.outEdges) == 0:
                self.addSinkEdge(edgeObj)

    def checkNet(self, forcedSourceSinkDetection):
        self.trimNet()
        for edge in self._edges.itervalues():
            if len(edge.detFlow) > 0:
                edge.capacity = max(edge.detFlow)
            if not options.respectzero and edge.capacity == 0:
                edge.capacity = sys.maxint
            edge.startCapacity = edge.capacity
        if not forcedSourceSinkDetection:
            if len(self._sink.inEdges) == 0:
                warn("Warning! No sinks, trying to find some.")
                forcedSourceSinkDetection = True
            if len(self._source.outEdges) == 0:
                warn("Warning! No sources, trying to find some.")
                forcedSourceSinkDetection = True
        if forcedSourceSinkDetection:
            self.detectSourceSink()
        if len(self._sink.inEdges) == 0:
            print "Error! No sinks found."
            return False
        if len(self._source.outEdges) == 0:
            print "Error! No sources found."
            return False
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
            print len(self._source.outEdges), "sources,", unlimitedSource, "unlimited"
            print len(self._sink.inEdges), "sinks,", unlimitedSink, "unlimited"
        return True

    def updateFlow(self, pred):
        flowDelta = sys.maxint
        currVertex = self._sink
        while not currVertex == self._source:
            currEdge = pred[currVertex]
            if currEdge.target == currVertex:
                flowDelta = min(flowDelta, currEdge.capacity - currEdge.flow)
                currVertex = currEdge.source
            else:
                flowDelta = min(flowDelta, currEdge.flow)
                currVertex = currEdge.target
        assert flowDelta < sys.maxint
        currVertex = self._sink
        while not currVertex == self._source:
            currEdge = pred[currVertex]
            if currEdge.target == currVertex:
                currEdge.flow += flowDelta
                currVertex = currEdge.source
            else:
                currEdge.flow -= flowDelta
                currVertex = currEdge.target

    def adaptCapacities(self):
        pathFound = True
        while pathFound:
            pathFound = False
            queue = [self._source]
            unlimited = set(queue)
            pred = {}
            while len(queue) > 0:
                currVertex = queue.pop(0)
                if currVertex == self._sink:
                    self.updateFlow(pred)
                    pathFound = True
                    break
                for edge in currVertex.outEdges:
                    if not edge.target in pred and edge.flow < edge.capacity:
                        if edge.target != self._sink or not currVertex in unlimited:
                            queue.append(edge.target)
                            pred[edge.target] = edge
                            if currVertex in unlimited:
                                if edge.capacity == sys.maxint:
                                    unlimited.add(edge.target)
                                else:
                                    unlimited.discard(edge.target)
                for edge in currVertex.inEdges:
                    if not edge.source in pred and edge.flow > 0:
                        queue.append(edge.source)
                        pred[edge.source] = edge
                        unlimited.discard(edge.source)
        for vertex in self._vertices:
            for succEdge in vertex.outEdges:
                succEdge.capacity = succEdge.flow
                succEdge.flow = 0

    def printRouteAndUpdateFlow(self, pred, endEdge):
        route = ''
        flowDelta = sys.maxint
        currEdge = endEdge
        while not currEdge.source == self._source:
            if currEdge.kind == "real":
                if route == '':
                    if currEdge.finalizer:
                        route = currEdge.finalizer
                    else:
                        warn("Warning! No finalizer for " + currEdge.label + ". Route will be one edge too short.")
                route = currEdge.label + " " + route
            flowDelta = min(flowDelta, currEdge.capacity - currEdge.flow)
            currEdge = pred[currEdge]
        flowDelta = min(flowDelta, currEdge.capacity - currEdge.flow)
        currEdge = endEdge
        firstReal = ''
        while not currEdge.source == self._source:
            currEdge.flow += flowDelta
            currEdge = pred[currEdge]
            if currEdge.kind == "real":
                firstReal = currEdge.label
        currEdge.flow += flowDelta
        route = route.strip()
        assert firstReal != ''
        if self._routeOut:
            if not firstReal in self._routes:
                self._routes[firstReal] = []
            routeID = firstReal+"."+str(len(self._routes[firstReal]))
            self._routes[firstReal].append(routeID)
            self._routeFreq[routeID] = flowDelta
            self._routeOut.write('    <route id="'+routeID+'" multi_ref="x">')
            self._routeOut.write(route+'</route>\n')
        else:
            print route, flowDelta

    def calcRoutes(self):
        self.adaptCapacities()
        if options.routefile:
            self._routeOut = open(options.routefile, 'w')
            self._routeOut.write("<routes>\n")
        routeFound = True
        while routeFound:
            routeFound = False
            queue = list(self._source.outEdges)
            pred = {}
            while len(queue) > 0:
                currEdge = queue.pop(0)
                if currEdge.kind != "real" or currEdge.flow < currEdge.capacity:
                    if currEdge.target == self._sink:
                        routeFound = True
                        self.printRouteAndUpdateFlow(pred, currEdge)
                        break
                    for edge in currEdge.target.outEdges:
                        if not edge in pred:
                            pred[edge] = currEdge
                            queue.append(edge)
        if options.routefile:
            self._routeOut.write("</routes>\n")
            self._routeOut.close()
        for vertex in self._vertices:
            for succEdge in vertex.outEdges:
                assert succEdge.capacity == succEdge.flow

    def writeEmitters(self, emitFileName):
        emitOut = open(emitFileName, 'w')
        print >> emitOut, "<additional>"
        for edgeLabel, routes in self._routes.iteritems():
            srcFile = "src_"+edgeLabel + ".def.xml"
            print >> emitOut, '    <trigger id="src_' + edgeLabel + '"',
            print >> emitOut, 'objecttype="emitter" pos="0"',
            print >> emitOut, 'friendly_pos="x" objectid="' + edgeLabel + '_0"',
            print >> emitOut, 'file="' + srcFile + '"/>'
            srcOut = open(srcFile, 'w')
            print >> srcOut, "<triggeredsource>"
            for route in routes:
                print >> srcOut, '    <routedistelem id="' + route + '"',
                print >> srcOut, 'probability="' + str(self._routeFreq[route]) + '"/>'
            for time in range(self._edges[edgeLabel].flow):
                print >> srcOut, '    <emit time="' + str(time) + '"/>'
            print >> srcOut, "</triggeredsource>"
            srcOut.close()
        print >> emitOut, "</additional>"
        emitOut.close()

    def writeFlowPOIs(self, poiFileName):
        poiOut = open(poiFileName, 'w')
        print >> poiOut, "<pois>"
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
            print >> poiOut, '    <poi id="' + label + '_f' + flow + 'c' + cap + '"',
            print >> poiOut, 'color = "' + color + '" lane="' + label + '_0" pos="0"/>'
        print >> poiOut, "</pois>"
        poiOut.close()


class NetDetectorFlowReader(handler.ContentHandler):

    def __init__(self, net):
        self._net = net
        self._edgeString = ''
        self._edge = ''
        self._lane2edge = {}
        self._det2edge = {}

    def startElement(self, name, attrs):
        if name == 'edges':
            self._edgeString = ' '
        if name == 'edge' and (not 'function' in attrs or attrs['function'] != 'internal'):
            self._edge = attrs['id']
        if name == 'cedge':
            fromEdge = self._net.getEdge(self._edge)
            toEdge = self._net.getEdge(attrs['id'])
            newEdge = Edge(self._edge+"_"+attrs['id'], fromEdge.target, toEdge.source)
            self._net.addEdge(newEdge)
            fromEdge.finalizer = attrs['id']
        if name == 'lane':
            self._lane2edge[attrs['id']] = self._edge
            edgeObj = self._net.getEdge(self._edge)
            edgeObj.maxSpeed = max(edgeObj.maxSpeed, float(attrs['maxspeed']))
            edgeObj.laneCount += 1
        if name == 'detector_definition':
            if not attrs['lane'] in self._lane2edge:
                warn("Warning! Unknown lane " + attrs['lane'] + ", ignoring " + attrs['id'])
                return
            edgeObj = self._net.getEdge(self._lane2edge[attrs['lane']])
            edgeObj.addDet(float(attrs['pos']), attrs['id'])
            self._det2edge[attrs['id']] = edgeObj
            if not 'type' in attrs:
                if not options.sourcesink:
                    warn("Warning! No type for detector " + attrs['id'])
            else:
                if attrs['type'] == 'source':
                    self._net.addSourceEdge(edgeObj)
                if attrs['type'] == 'sink':
                    self._net.addSinkEdge(edgeObj)

    def characters(self, content):
        if self._edgeString != '':
            self._edgeString += content

    def endElement(self, name):
        if name == 'edges':
            for edge in self._edgeString.split():
                self._net.addIsolatedRealEdge(edge)
            self._edgeString = ''

    def readFlows(self, flowFile):
        headerSeen = False
        for l in file(flowFile):
            flowDef = l.split(';')
            if not headerSeen and flowDef[0] == "Detector":
                headerSeen = True
                continue
            if not flowDef[0] in self._det2edge:
                warn("Warning! Unknown detector " + flowDef[0])
            else:
                self._det2edge[flowDef[0]].addFlow(flowDef[0], int(flowDef[2]))


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
optParser.add_option("-o", "--routes-output", dest="routefile",
                     help="write routes to FILE", metavar="FILE")
optParser.add_option("-e", "--emitters-output", dest="emitfile",
                     help="write emitters to FILE and create files per emitter (needs -o)", metavar="FILE")
optParser.add_option("-t", "--trimmed-output", dest="trimfile",
                     help="write edges of trimmed network to FILE", metavar="FILE")
optParser.add_option("-p", "--flow-poi-output", dest="flowpoifile",
                     help="write resulting flows as SUMO POIs to FILE", metavar="FILE")
optParser.add_option("-m", "--min-speed", type="float", dest="minspeed",
                     default=0.0, help="only consider edges where the fastest lane allows at least this maxspeed (m/s), together with their predecessors and successors")
optParser.add_option("-D", "--keep-det", action="store_true", dest="keepdet",
                     default=False, help='keep edges with detectors when deleting "slow" edges')
optParser.add_option("-s", "--source-sink-detection", action="store_true", dest="sourcesink",
                     default=False, help="detect sources and sinks")
optParser.add_option("-z", "--respect-zero", action="store_true", dest="respectzero",
                     default=False, help="respect detectors without data (or with permanent zero) with zero flow")
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
parser.parse(options.detfile)
if options.verbose:
    print "Reading flows"
for flow in options.flowfiles:
    reader.readFlows(flow)
if net.checkNet(options.sourcesink):
    if options.verbose:
        print "Calculating routes"
    net.calcRoutes()
    if options.emitfile:
        net.writeEmitters(options.emitfile)
    if options.flowpoifile:
        net.writeFlowPOIs(options.flowpoifile)
