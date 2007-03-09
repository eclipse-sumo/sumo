#!/usr/bin/python
# This script does simple checks for the routes on a given network.
# It needs at least two parameters, the SUMO net (.net.xml) and a file
# specifying vehicle types and routes. If the vehicle types and the
# routes are defined in separate files, the vehicle types file should
# be the second parameter and the routes files the third and subsequent.
# At the moment it is not possible to specify multiple vehicle types files.
# Warnings will be issued if there is an unknown esge in the route,
# if the route is too short (only one edge), if the route is disconnected
# or if the first edge is too short for the vehicle.
import sys

from xml.sax import make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edge = ''
        self._nb = {}
        self._edgeL = {}

    def startElement(self, name, attrs):
        if name == 'edge' and (not 'function' in attrs or attrs['function'] != 'internal'):
            self._edge = attrs['id']
            self._nb[self._edge] = set()
            self._edgeL[self._edge] = float(attrs['Length'])
        if name == 'cedge':
            self._nb[self._edge].add(attrs['id'])

    def hasEdge(self, edge):
        return edge in self._nb

    def isNeighbor(self, orig, dest):
        return dest in self._nb[orig]

    def getLength(self, edge):
        return self._edgeL[edge]

            
class VehicleTypeReader(handler.ContentHandler):

    def __init__(self):
        self._type = ''
        self._length = {}
        self._minLength = 100000.

    def startElement(self, name, attrs):
        if name == 'vtype':
            self._length[attrs['id']] = float(attrs['length'])
            if float(attrs['length']) < self._minLength:
                self._minLength = float(attrs['length'])

    def isCarLongerThanEdge(self, vtype, edgeL):
        return self._length[vtype] > edgeL
                    
    def isEveryCarLongerThanEdge(self, edgeL):
        return self._minLength > edgeL
                    

class RouteReader(handler.ContentHandler):

    def __init__(self, net, types):
        self._vType = ''
        self._vID = ''
        self._routeID = ''
        self._routeString = ''
        self._net = net
        self._types = types

    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vType = attrs['type']
            self._vID = attrs['id']
        if name == 'route':
            if 'id' in attrs:
                self._routeID = attrs['id']
            else:
                self._routeID = "for vehicle " + self._vID
            self._routeString = ''

    def endElement(self, name):
        if name == 'route':
            self.testRoute()
            self._routeID = ''
            self._routeString = ''
        if name == 'vehicle':
            self._vType = ''

    def characters(self, content):
        self._routeString += content

    def testRoute(self):
        if self._routeID != '':
            rdata = self._routeString.split()
            if len(rdata) < 2:
                print "Warning: Route " + self._routeID + " too short"
            doConnectivityTest = True
            doLengthTest = (len(rdata) > 0)
            for v in rdata:
                if not self._net.hasEdge(v):
                    print "Warning: Unknown edge " + v + " in route " + self._routeID
                    doConnectivityTest = False
                    if v == rdata[0]:
                        doLengthTest = False
            if doConnectivityTest:
                for i, v in enumerate(rdata):
                    if i < len(rdata) - 1:
                        if not self._net.isNeighbor(v, rdata[i+1]):
                            print "Warning: Route " + self._routeID + " disconnected between " + v + " and " + rdata[i+1]
            if doLengthTest:
                edgeL = self._net.getLength(rdata[0]);
                if self._vType != '':
                    if self._types.isCarLongerThanEdge(self._vType, edgeL):
                        print "Warning: Vehicle too long for " + rdata[0] + ", first edge of route " + self._routeID
                else:
                    if self._types.isEveryCarLongerThanEdge(edgeL):
                        print "Warning: All vehicles too long for " + rdata[0] + ", first edge of route " + self._routeID
                    

            
if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <net> <vehicletypes> <routes>+"
    print "    or " + sys.argv[0] + " <net> <vehicletypes+routes>"
    sys.exit()
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
parser = make_parser()
types = VehicleTypeReader()
parser.setContentHandler(types)
parser.parse(sys.argv[2])
parser = make_parser()
parser.setContentHandler(RouteReader(net, types))
if len(sys.argv) > 3:
    for f in sys.argv[3:]:
        print "checking " + f
        parser.parse(f)
else:
    parser.parse(sys.argv[2])
