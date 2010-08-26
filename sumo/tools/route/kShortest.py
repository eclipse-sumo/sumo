#!/usr/bin/env python
import os, random, string, sys

from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser

class Predecessor:

    def __init__(self, edge, pred, distance):
        self.edge = edge
        self.pred = pred
        self.distance = distance


class Vertex:

    def __init__(self):
        self.inEdges = set()
        self.outEdges = set()
        self.preds = []
        self.wasUpdated = False

    def _addNewPredecessor(self, edge, updatePred, newPreds):
        for pred in newPreds:
            if pred.pred == updatePred:
                return
        pred = updatePred
        if options.noloops:
            while pred != None:
                if pred.edge == edge:
                    return
                pred = pred.pred
        newPreds.append(Predecessor(edge, updatePred,
                                    updatePred.distance + edge.weight))

    def update(self, edge):
        updatePreds = edge.source.preds
        if len(self.preds) == options.k\
           and updatePreds[0].distance + edge.weight >= self.preds[options.k-1].distance:
            return False
        newPreds = []
        updateIndex = 0
        predIndex = 0
        while len(newPreds) < options.k\
              and (updateIndex < len(updatePreds)\
                   or predIndex < len(self.preds)):
            if predIndex == len(self.preds):
                self._addNewPredecessor(edge, updatePreds[updateIndex], newPreds)
                updateIndex += 1
            elif updateIndex == len(updatePreds):
                newPreds.append(self.preds[predIndex])
                predIndex += 1
            elif updatePreds[updateIndex].distance + edge.weight < self.preds[predIndex].distance:
                self._addNewPredecessor(edge, updatePreds[updateIndex], newPreds)
                updateIndex += 1
            else:
                newPreds.append(self.preds[predIndex])
                predIndex += 1
        if predIndex == len(newPreds): # no new added
            return False
        self.preds = newPreds
        returnVal = not self.wasUpdated
        self.wasUpdated = True
        return returnVal


class Edge:

    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.kind = kind
        self.weight = 0

    def __repr__(self):
        return self.kind+"_"+self.label + "<%s>" % self.weight


class Net:

    def __init__(self):
        self._vertices = []
        self._edges = {}

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

    def addIsolatedRealEdge(self, edgeLabel):
        self.addEdge(Edge(edgeLabel, self.newVertex(), self.newVertex(),
                          "real"))

    def calcPaths(self, startEdgeLabel):
        if startEdgeLabel:
            startVertex = self._edges[startEdgeLabel].source
        else:
            startVertex = self._vertices[0]
        startVertex.preds.append(Predecessor(None, None, 0))
        updatedVertices = [startVertex]
        while len(updatedVertices) > 0:
            vertex = updatedVertices.pop(0)
            vertex.wasUpdated = False
            for edge in vertex.outEdges:
                if edge.target != startVertex and edge.target.update(edge):
                    updatedVertices.append(edge.target)
        self.printRoutes(startVertex)

    def printRoutes(self, startVertex):
        if options.traveltime:
            weight="duration"
        else:
            weight="length"
        print "<routes>"
        for lastVertex in self._vertices:
            for num, startPred in enumerate(lastVertex.preds):
                vertex = lastVertex  
                pred = startPred
                route = ""
                lastEdge = None
                firstEdge = None
                while vertex != startVertex:
                    if pred.edge.kind == "real":
                        firstEdge = pred.edge
                        if not lastEdge:
                            lastEdge = pred.edge
                        route = pred.edge.label + " " + route
                    vertex = pred.edge.source
                    pred = pred.pred
                if lastEdge != firstEdge:
                    print '    <route id="route%s_%s_%s" %s="%s" edges="%s"/>'\
                          % (num, firstEdge.label, lastEdge.label,
                             weight, startPred.distance, route[:-1])
            print
        print "</routes>"


class NetReader(handler.ContentHandler):

    def __init__(self, net):
        self._net = net
        self._edge = ''

    def startElement(self, name, attrs):
        if name == 'edge' and (not attrs.has_key('function') or attrs['function'] != 'internal'):
            self._edge = attrs['id']
            self._net.addIsolatedRealEdge(attrs['id'])
        elif name == 'succ':
            self._edge = attrs['edge']
            if self._edge[0]==':':
                self._edge = ''
        elif name == 'succlane' and self._edge != '':
            fromEdge = self._net.getEdge(self._edge)
            l = attrs['lane']
            if l!="SUMO_NO_DESTINATION":
                toEdge = self._net.getEdge(l[:l.rfind('_')])
                newEdge = Edge(self._edge+"_"+l[:l.rfind('_')], fromEdge.target, toEdge.source)
                self._net.addEdge(newEdge)
        elif name == 'lane' and self._edge != '':
            edgeObj = self._net.getEdge(self._edge)
            if options.traveltime:
                edgeObj.weight = float(attrs['length']) / float(attrs['maxspeed'])
            else:
                edgeObj.weight = float(attrs['length'])

    def endElement(self, name):
        if name == 'edge':
            self._edge = ''


optParser = OptionParser()
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="read SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-k", "--num-paths", type="int", dest="k", metavar="NUM",
                     default=3, help="calculate the shortest k paths [default: %default]")
optParser.add_option("-s", "--start-edge", dest="start", default="",
                     help="start at the start vertex of this edge")
optParser.add_option("-t", "--travel-time", action="store_true", dest="traveltime",
                     help="use minimum travel time instead of length")
optParser.add_option("-l", "--no-loops", action="store_true", dest="noloops",
                     default=False, help="omit routes which travel an edge twice")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
if not options.netfile:
    optParser.print_help()
    sys.exit()
parser = make_parser()
if options.verbose:
    print "Reading net"
net = Net()
reader = NetReader(net)
parser.setContentHandler(reader)
parser.parse(options.netfile)
if options.verbose:
    print len(net._edges), "edges read"
net.calcPaths(options.start)
