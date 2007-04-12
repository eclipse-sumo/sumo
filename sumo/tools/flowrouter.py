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

class Edge:

    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.capacity = sys.maxint
        self.flow = 0
        self.kind = kind
        self.maxSpeed = 0.0
        self.finalizer = None

    def __repr__(self):
        cap = str(self.capacity)
        if self.capacity == sys.maxint:
            cap = "inf"
        return self.kind+"_"+self.label+"<"+str(self.flow)+"|"+cap+">"
    
        
class Net:

    def __init__(self):
        self._currVertex = 0
        self._edges = {}
        self._inEdges = {}
        self._outEdges = {}
        self._source = self.newVertex()
        self._sink = self.newVertex()
        self._routeOut = None
        self._routes = {}
        self._routeFreq = {}

    def newVertex(self):
        self._currVertex += 1
        self._inEdges[self._currVertex] = set()
        self._outEdges[self._currVertex] = set()
        return self._currVertex

    def getEdge(self, edgeLabel):
        return self._edges[edgeLabel]

    def addEdge(self, edgeObj):
        self._outEdges[edgeObj.source].add(edgeObj)
        self._inEdges[edgeObj.target].add(edgeObj)
        if edgeObj.kind == "real":
            self._edges[edgeObj.label] = edgeObj
                
    def removeEdge(self, edgeObj):
        self._outEdges[edgeObj.source].remove(edgeObj)
        self._inEdges[edgeObj.target].remove(edgeObj)
        if edgeObj.kind == "real":
            del self._edges[edgeObj.label]
            checkEdges = self._inEdges[edgeObj.source].union(self._outEdges[edgeObj.target])
            for edge in checkEdges:
                if edge.kind != "real":
                    self.removeEdge(edge)
                
    def addIsolatedRealEdge(self, edgeLabel):
        self.addEdge(Edge(edgeLabel, self.newVertex(), self.newVertex(), "real"))
                
    def addSourceEdge(self, edgeLabel):
        edgeSource = self._edges[edgeLabel].source
        newEdge = Edge("s_"+edgeLabel, self._source, edgeSource, "source")
        self.addEdge(newEdge)
                
    def addSinkEdge(self, edgeLabel):
        edgeTarget = self._edges[edgeLabel].target
        newEdge = Edge("t_"+edgeLabel, edgeTarget, self._sink, "sink")
        self.addEdge(newEdge)

    def compressNet(self):
        if options.minspeed > 0.0:
            if options.verbose:
                print "Removing edges with maxspeed <", options.minspeed, "... ", 
            for edge in self._edges.values():
                if edge.maxSpeed < options.minspeed:
                    delete = True
                    if options.keepsucc:
                        for auxpred in self._inEdges[edge.source]:
                            for realpred in self._inEdges[auxpred.source]:
                                if realpred.maxSpeed >= options.minspeed:
                                    delete = False
                                    break
                            if not delete: break
                    if delete and options.keeppred:
                        for auxsucc in self._outEdges[edge.target]:
                            for realsucc in self._outEdges[auxsucc.target]:
                                if realsucc.maxSpeed >= options.minspeed:
                                    delete = False
                                    break
                            if not delete: break
                    if options.keepdet and edge.capacity < sys.maxint:
                        delete = False
                    if delete:
                        self.removeEdge(edge)
            if options.verbose:
                print len(self._edges), "left"
        
    def detectSourceSink(self):
        for edge in self._edges.values():
            if len(self._inEdges[edge.source]) == 0:
                self.addSourceEdge(edge.label)
            if len(self._outEdges[edge.target]) == 0:
                self.addSinkEdge(edge.label)
        
    def checkNet(self, forcedSourceSinkDetection):
        self.compressNet()
        if options.ignorezero:
            for edge in self._edges.values():
                if edge.capacity == 0:
                    edge.capacity = sys.maxint
        if not forcedSourceSinkDetection:
            if len(self._inEdges[self._sink]) == 0:
                warn("Warning! No sinks, trying to find some.")
                forcedSourceSinkDetection = True
            if len(self._outEdges[self._source]) == 0:
                warn("Warning! No sources, trying to find some.")
                forcedSourceSinkDetection = True
        if forcedSourceSinkDetection:
            self.detectSourceSink()
        sourceEdges = list(self._outEdges[self._source])
        for source in sourceEdges:
            queue = [source.target]
            seenVertices = set()
            while len(queue) > 0:
                currVertex = queue.pop(0)
                if currVertex == self._sink:
                    warn("Warning! Source " + source.label + " leads to path with unlimited capacity, ignoring it.")
                    self.removeEdge(source)
                    break
                seenVertices.add(currVertex)
                for edge in self._outEdges[currVertex]:
                    if not edge.target in seenVertices and edge.capacity == sys.maxint:
                        queue.append(edge.target)
        if len(self._inEdges[self._sink]) == 0:
            print "Error! No sinks found."
            return False
        if len(self._outEdges[self._source]) == 0:
            print "Error! No sources found."
            return False
        if options.verbose:
            print len(self._outEdges[self._source]), "sources", len(self._inEdges[self._sink]), "sinks"
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
            pred = {}
            seenVertices = set()
            while len(queue) > 0:
                currVertex = queue.pop(0)
                if currVertex == self._sink:
                    self.updateFlow(pred)
                    pathFound = True
                    break
                seenVertices.add(currVertex)
                for edge in self._outEdges[currVertex]:
                    if not edge.target in seenVertices and edge.flow < edge.capacity:
                        queue.append(edge.target)
                        pred[edge.target] = edge
                for edge in self._inEdges[currVertex]:
                    if not edge.source in seenVertices and edge.flow > 0:
                        queue.append(edge.source)
                        pred[edge.source] = edge
        for edge in self._edges.itervalues():
            edge.capacity = edge.flow
            edge.flow = 0
            for succEdge in self._outEdges[edge.target]:
                if succEdge.kind != "real":
                    succEdge.capacity = succEdge.flow
                    succEdge.flow = 0
        for srcEdge in self._outEdges[self._source]:
            if srcEdge.kind != "real":
                srcEdge.capacity = srcEdge.flow
                srcEdge.flow = 0

    def printRouteAndUpdateFlow(self, pred, endEdge):
        route = ''
        flowDelta = sys.maxint
        currEdge = endEdge
        while not currEdge.source == self._source:
            if currEdge.kind == "real":
                if route == '' and currEdge.finalizer:
                    route = currEdge.finalizer
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
            routeID = route.replace(' ', '_')
            if not firstReal in self._routes:
                self._routes[firstReal] = []
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
            queue = list(self._outEdges[self._source])
            pred = {}
            seenVertices = set()
            seenVertices.add(self._source)
            while len(queue) > 0:
                currEdge = queue.pop(0)
                if currEdge.kind != "real" or currEdge.flow < currEdge.capacity:
                    if currEdge.target == self._sink:
                        routeFound = True
                        self.printRouteAndUpdateFlow(pred, currEdge)
                        break
                    for edge in self._outEdges[currEdge.target]:
                        if not edge.target in seenVertices:
                            pred[edge] = currEdge
                            queue.append(edge)
        if options.routefile:
            self._routeOut.write("</routes>\n")
            self._routeOut.close()
        if options.emitfile:
            emitOut = open(options.emitfile, 'w')
            emitOut.write("<additional>\n")
            for edgeLabel, routes in self._routes.iteritems():
                srcFile = "src_"+edgeLabel + ".def.xml"
                emitOut.write('    <trigger id="src_' + edgeLabel + '" objecttype="emitter" ')
                emitOut.write('pos="0" friendly_pos="x" ')
                emitOut.write('objectid="' + edgeLabel + '_0" file="' + srcFile + '"/>\n')
                srcOut = open(srcFile, 'w')
                srcOut.write("<triggeredsource>\n")
                for route in routes:
                    srcOut.write('    <routedistelem id="'+route+'" probability="'+str(self._routeFreq[route])+'"/>\n')
                print self._edges[edgeLabel].flow
                for time in range(self._edges[edgeLabel].flow):
                    srcOut.write('    <emit time="'+str(time)+'"/>\n')                                     
                srcOut.write("</triggeredsource>\n")
                srcOut.close()
            emitOut.write("</additional>\n")
            emitOut.close()


class NetDetectorFlowReader(handler.ContentHandler):

    def __init__(self, net):
        self._net = net
        self._edgeString = ''
        self._edge = ''
        self._lane2edge = {}
        self._edgeDetPos = {}
        self._det2edge = {}
        self._detIgnore = set()

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
        if name == 'detector_definition':
            if not attrs['lane'] in self._lane2edge:
                warn("Warning! Unknown lane " + attrs['lane'] + ", ignoring " + attrs['id'])
                return
            edge = self._lane2edge[attrs['lane']]
            pos = float(attrs['pos'])
            if edge in self._edgeDetPos and abs(self._edgeDetPos[edge] - pos) > MAX_POS_DEVIATION:
                warn("Warning! Edge " + edge + " has detectors at multiple positions, ignoring " + attrs['id'])
                self._detIgnore.add(attrs['id'])
            else:
                self._edgeDetPos[edge] = pos
                self._det2edge[attrs['id']] = edge
            if not 'type' in attrs:
                if not options.sourcesink:
                    warn("Warning! No type for detector " + attrs['id'])
            else:
                if attrs['type'] == 'source':
                    self._net.addSourceEdge(edge)
                if attrs['type'] == 'sink':
                    self._net.addSinkEdge(edge)

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
            index = 2
            if options.whitespace:
                flowDef = l.split()
                index = 1
            else:
                flowDef = l.split(';')
            if not headerSeen and flowDef[0] == "Detector":
                headerSeen = True
                continue
            if not flowDef[0] in self._det2edge:
                if not flowDef[0] in self._detIgnore and options.unknowndet:
                    warn("Warning! Unknown detector " + flowDef[0])
            else:
                edge = self._net.getEdge(self._det2edge[flowDef[0]])
                if edge.capacity == sys.maxint:
                    edge.capacity = int(flowDef[index])
                else:
                    edge.capacity += int(flowDef[index])


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
                     help="write emitters to FILE and create files per emitter", metavar="FILE")
optParser.add_option("-m", "--min-speed", type="float", dest="minspeed",
                     default=0.0, help="only consider edges where the fastest lane allows at least this maxspeed (m/s)")
optParser.add_option("-P", "--keep-pred", action="store_true", dest="keeppred",
                     default=False, help='keep predecessors of "fast" edges when deleting "slow" edges')
optParser.add_option("-S", "--keep-succ", action="store_true", dest="keepsucc",
                     default=False, help='keep successors of "fast" edges when deleting "slow" edges')
optParser.add_option("-D", "--keep-det", action="store_true", dest="keepdet",
                     default=False, help='keep pedges with detectors when deleting "slow" edges')
optParser.add_option("-s", "--source-sink-detection", action="store_true", dest="sourcesink",
                     default=False, help="detect sources and sinks")
optParser.add_option("-w", "--whitespace-separated", action="store_true", dest="whitespace",
                     default=False, help="entries in flow files are whitespace separated, without time column")
optParser.add_option("-u", "--unknown-detectors", action="store_true", dest="unknowndet",
                     default=False, help="warn about unknown detectors")
optParser.add_option("-i", "--ignore-zero", action="store_true", dest="ignorezero",
                     default=False, help="ignore detectors with zero flow")
optParser.add_option("-q", "--quiet", action="store_true", dest="quiet",
                     default=False, help="suppress warnings")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.netfile or not options.detfile or not options.flowfiles:
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
