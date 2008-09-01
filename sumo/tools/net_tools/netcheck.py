#!/usr/bin/python
# This script does simple checks for the network.
# It tests whether the network is (weakly) connected
# and optionally whether the routes cover the net.
# It needs at least one parameter, the SUMO net (.net.xml).
# All parameters starting from the second are considered route files.
import os, string, sys, StringIO

from xml.sax import saxutils, make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edge = ''
        self._nb = {}
        self._components = list()

    def startElement(self, name, attrs):
        if name == 'edge' and (not attrs.has_key('function') or attrs['function'] != 'internal'):
            self._nb[edge] = set()
        elif name == 'succ':
            self._edge = attrs['edge']
            if self._edge[0]==':':
                self._edge = ''
        elif name == 'succlane' and self._edge != '':
            id = attrs['lane']
            if id!="SUMO_NO_DESTINATION":
                id = self._net.getEdge(id[:id.rfind('_')])
                self._nb[self._edge].add(id)
                self._nb[id].add(self._edge)

    def isWeaklyConnected(self):
        if self.getNumEdges() == 0:
            return True
        edgesLeft = set(self._nb.keys())
        queue = list()
        while not len(edgesLeft) == 0:
            component = set()
            queue.append(edgesLeft.pop())
            while not len(queue) == 0:
                edge = queue.pop(0)
                component.add(edge)
                for n in self._nb[edge]:
                    if n in edgesLeft:
                        queue.append(n)
                        edgesLeft.remove(n)
            self._components.append(component)
        return len(self._components) == 1

    def printComponents(self):
        if len(self._components) == 0:
            self.isWeaklyConnected()
        index = 0
        for comp in self._components:
            index = index + 1
            print "Component", index
            for edge in comp:
                print edge, " ",
            print

    def getNumEdges(self):
        return len(self._nb)

            
class RouteReader(handler.ContentHandler):

    def __init__(self):
        self._routeString = ''
        self._edgesCovered = set()
        
    def startElement(self, name, attrs):
        if name == 'route':
            self._routeString = ' '

    def endElement(self, name):
        if name == 'route':
            self.coverRoute()
            self._routeString = ''

    def characters(self, content):
        if self._routeString != ' ':
            self._routeString += content

    def coverRoute(self):
        for edge in self._routeString.split():
            self._edgesCovered.add(edge)
                    
    def getNumCovered(self):
        return len(self._edgesCovered)

            
if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net> <routes>*"
    sys.exit()
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
if not net.isWeaklyConnected():
    print "Warning! Net is not connected."
    net.printComponents()
if len(sys.argv) > 2:
    parser = make_parser()
    routes = RouteReader()
    parser.setContentHandler(routes)
    for f in sys.argv[2:]:
        parser.parse(f)
    if net.getNumEdges() > routes.getNumCovered:
        print "Warning! Routes do not cover the net."
