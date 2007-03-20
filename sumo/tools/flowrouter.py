#!/usr/bin/python
# This script does flow routing similar to the dfrouter.
# It needs three parameters, the SUMO net (.net.xml), a file
# specifying detectors (with types!) and one for the flows.
import os, string, sys, StringIO

from xml.sax import saxutils, make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edge = ''
        self._inEdges = {}
        self._outEdges = {}
        self._lane2edge = {}
        self._edge2det = {}
        self._det2edge = {}
        self._edgeCap = {}
        self._edgeFlow = {}
        self._sourceEdges = set()
        self._sinkEdges = set()
        self._edgeString = ''

    def startElement(self, name, attrs):
        if name == 'edges':
            self._edgeString = ' '
        if name == 'edge' and (not 'function' in attrs or attrs['function'] != 'internal'):
            self._edge = attrs['id']
        if name == 'cedge':
            self._outEdges[self._edge].add(attrs['id'])
            self._inEdges[attrs['id']].add(self._edge)
        if name == 'lane':
            self._lane2edge[attrs['id']] = self._edge
        if name == 'detector_definition':
            edge = self._lane2edge[attrs['lane']]
            if edge in self._edge2det:
                print "Warning! Edge " + edge + " has detector " + self._edge2det[edge] + ". Ignoring " + attrs['id']
            else:
                self._edge2det[edge] = attrs['id']
                self._det2edge[attrs['id']] = edge
            if not 'type' in attrs:
                print "Warning! No type for detector " + attrs['id']
            else:
                if attrs['type'] == 'source':
                    self._sourceEdges.add(edge)
                if attrs['type'] == 'sink':
                    self._sinkEdges.add(edge)

    def characters(self, content):
        if self._edgeString != '':
            self._edgeString += content

    def endElement(self, name):
        if name == 'edges':
            for edge in self._edgeString.split():
                self._inEdges[edge] = set()
                self._outEdges[edge] = set()
            self._edgeString = ''

    def readFlows(self, flowFile):
        for l in file(flowFile):
            flowDef = l.split(';')
            if not flowDef[0] in self._det2edge:
                print "Warning! Unknown detector " + flowDef[0]
            else:
                self._edgeCap[self._det2edge[flowDef[0]]] = int(flowDef[2])

    def printCaps(self):
        print self._sourceEdges
        print self._edgeCap
        print self._sinkEdges

##    def adaptCapacities(self):
##        self._edgeFlow.clear()
##        routeFound = True
##        while routeFound:
##            routeFound = False
##            queue = list(self._sourceEdges)
##            while len(queue) > 0:
##                currEdge = queue.pop(0)
##                if currEdge in self._edgeCap:
##                    if not currEdge in self._edgeFlow:
##                        self._edgeFlow[currEdge] = 0
##                    if self._edgeCap[currEdge] > self._edgeFlow[currEdge]:
##                        if currEdge in self._sinkEdges:
##                            routeFound = True
##                            break
##                        for edge in self._outEdges[currEdge]:
##                            if not edge in queue:
##                                pred[edge] = currEdge
##                                queue.append(edge)
##            if routeFound:
##                self.printRouteAndUpdateFlow(pred, currEdge)

    def printRouteAndUpdateFlow(self, pred, currEdge):
        route = ''
        flowDelta = sys.maxint
        while not currEdge in self._sourceEdges:
            route = currEdge + " " + route
            flowDelta = min(flowDelta, self._edgeCap[currEdge] - self._edgeFlow[currEdge])
            currEdge = pred[currEdge]
        route = currEdge + " " + route
        flowDelta = min(flowDelta, self._edgeCap[currEdge] - self._edgeFlow[currEdge])
        for edge in route.split():
            self._edgeFlow[currEdge] = self._edgeFlow[currEdge] + flowDelta
        print route, flowDelta

    def calcRoutes(self):
##        self.adaptCapacities()
        self._edgeFlow.clear()
        routeFound = True
        while routeFound:
            routeFound = False
            queue = list(self._sourceEdges)
            pred = {}
            while len(queue) > 0:
                currEdge = queue.pop(0)
                if currEdge in self._edgeCap:
                    if not currEdge in self._edgeFlow:
                        self._edgeFlow[currEdge] = 0
                    if self._edgeCap[currEdge] > self._edgeFlow[currEdge]:
                        if currEdge in self._sinkEdges:
                            routeFound = True
                            break
                        for edge in self._outEdges[currEdge]:
                            if not edge in queue:
                                pred[edge] = currEdge
                                queue.append(edge)
            if routeFound:
                self.printRouteAndUpdateFlow(pred, currEdge)


            
if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " <net> <detectors> <flows>"
    sys.exit()
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
parser.parse(sys.argv[2])
net.readFlows(sys.argv[3])
##net.printCaps()
net.calcRoutes()
