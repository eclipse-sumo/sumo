#!/usr/bin/python
# This script does simple checks for the routes on a given network.
# It needs at least two parameters, the SUMO net (.net.xml) and a file
# specifying vehicle types and routes. If the vehicle types and the
# routes are defined in separate files, the vehicle types file should
# be the second parameter and the routes files the third and subsequent.
# At the moment it is not possible to specify multiple vehicle types files.
# Warnings will be issued if there is an unknown edge in the route,
# if the route is too short (only one edge), if the route is disconnected
# or if the first edge is too short for the vehicle.
# If one specifies -f or --fix all subsequent route files will be fixed
# (if possible). At the moment this means removing the first edge
# if it is too short, adding an intermediate edge if just one link is missing
# in a disconnected route, removing the whole route in any other case.
# All changes are documented within the output file which has the same name
# as the input file with an additional .fixed suffix.
import os, string, sys, StringIO

from xml.sax import saxutils, make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edge = ''
        self._nb = {}
        self._edgeLength = {}

    def startElement(self, name, attrs):
        if name == 'edge' and (not attrs.has_key('function') or attrs['function'] != 'internal'):
            self._edge = attrs['id']
            self._nb[self._edge] = set()
	    if attrs.has_key('Length'):
                self._edgeLength[self._edge] = float(attrs['Length'])
        elif name == 'cedge':
            self._nb[self._edge].add(attrs['id'])
        elif name == 'lane' and not self._edge in self._edgeLength:
            self._edgeLength[self._edge] = float(attrs['length'])

    def hasEdge(self, edge):
        return edge in self._nb

    def isNeighbor(self, orig, dest):
        return dest in self._nb[orig]

    def getLength(self, edge):
        return self._edgeLength[edge]

    def getIntermediateEdge(self, orig, dest):
        for inter in self._nb[orig]:
            if dest in self._nb[inter]:
                return inter
        return ''

            
class VehicleTypeReader(handler.ContentHandler):

    def __init__(self):
        self._length = {}
        self._minLength = 5. # length of the default car

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

    def __init__(self, net, types, outFileName):
        self._vType = ''
        self._vID = ''
        self._routeID = ''
        self._routeString = ''
        self._removedString = ''
        self._addedString = ''
        self._net = net
        self._types = types
        if outFileName != '':
            self._out = open(outFileName, 'w')
        else:
            self._out = None
        self._fileOut = None
        self._isRouteValid = True
        self._changed = False
        
    def startDocument(self):
        if self._out:
            self._out.write('<?xml version="1.0" encoding="iso-8859-1"?>\n')

    def endDocument(self):
        if self._out:
            self._out.close()
            if not self._changed:
                os.remove(self._out.name)

    def condOutputRedirect(self):
        if self._out and not self._fileOut:
            self._fileOut = self._out
            self._out = StringIO.StringIO()

    def endOutputRedirect(self):
        if self._fileOut:
            if not self._isRouteValid:
                self._changed = True
                self._fileOut.write("<!-- ")
            self._fileOut.write(self._out.getvalue())
            if not self._isRouteValid:
                self._fileOut.write(" -->")
            if self._removedString != '':
                self._fileOut.write("<!-- removed edges: " + self._removedString + "-->")
                self._removedString = ''
            if self._addedString != '':
                self._fileOut.write("<!-- added edges: " + self._addedString + "-->")
                self._addedString = ''
            self._out.close()
            self._out = self._fileOut
            self._fileOut = None

    def startElement(self, name, attrs):
        if name == 'vehicle' and not attrs.has_key('route'):
            self.condOutputRedirect()
            self._vType = attrs['type']
            self._vID = attrs['id']
        if name == 'route':
            self.condOutputRedirect()
            if attrs.has_key('id'):
                self._routeID = attrs['id']
            else:
                self._routeID = "for vehicle " + self._vID
            self._routeString = ''
        if self._out:
            self._out.write('<' + name)
            for (name, value) in attrs.items():
                self._out.write(' %s="%s"' % (name, saxutils.escape(value)))
            self._out.write('>')

    def endElement(self, name):
        routeSectionEnded = False
        if name == 'route':
            self._isRouteValid = self.testRoute()
            if self._out:
                self._out.write(self._routeString)
            self._routeID = ''
            self._routeString = ''
            if self._vID == '':
                routeSectionEnded = True
        if name == 'vehicle' and self._vID != '':
            self._vType = ''
            self._vID = ''
            routeSectionEnded = True
        if self._out:
            self._out.write('</%s>' % name)
        if routeSectionEnded:
            self.endOutputRedirect()

    def characters(self, content):
        if self._routeID != '':
            self._routeString += content
        elif self._out:
            self._out.write(saxutils.escape(content))

    def testRoute(self):
        if self._routeID != '':
            returnValue = True
            rdata = self._routeString.split()
            if len(rdata) < 2:
                print "Warning: Route " + self._routeID + " too short"
                returnValue = False
            doConnectivityTest = True
            doLengthTest = (len(rdata) > 0)
            for v in rdata:
                if not self._net.hasEdge(v):
                    print "Warning: Unknown edge " + v + " in route " + self._routeID
                    returnValue = False
                    doConnectivityTest = False
                    if v == rdata[0]:
                        doLengthTest = False
            while doConnectivityTest:
                doConnectivityTest = False
                for i, v in enumerate(rdata):
                    if i < len(rdata) - 1 and not self._net.isNeighbor(v, rdata[i+1]):
                        print "Warning: Route " + self._routeID + " disconnected between " + v + " and " + rdata[i+1]
                        interEdge = self._net.getIntermediateEdge(v, rdata[i+1])
                        if interEdge != '':
                            rdata.insert(i+1, interEdge)
                            self._changed = True
                            self._addedString += interEdge + " "
                            self._routeString = string.join(rdata)
                            doConnectivityTest = True
                            break
                        returnValue = False
            if doLengthTest:
                edgeL = self._net.getLength(rdata[0]);
                removeFirst = False
                if self._vType != '':
                    if self._types.isCarLongerThanEdge(self._vType, edgeL):
                        print "Warning: Vehicle too long for " + rdata[0] + ", first edge of route " + self._routeID
                        removeFirst = True
                else:
                    if self._types.isEveryCarLongerThanEdge(edgeL):
                        print "Warning: All vehicles too long for " + rdata[0] + ", first edge of route " + self._routeID
                        removeFirst = True
                if removeFirst and self._out:
                    self._changed = True
                    self._removedString += rdata[0] + " "
                    self._routeString = string.join(rdata[1:])
                    return self.testRoute()
            return returnValue
        return False
                    
    def ignorableWhitespace(self, content):
        if self._out:
            self._out.write(content)
        
    def processingInstruction(self, target, data):
        if self._out:
            self._out.write('<?%s %s?>' % (target, data))


            
if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <net> <vehicletypes> [-f|--fix] <routes>+"
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
fixMode = False
parser.setContentHandler(RouteReader(net, types, ''))
if len(sys.argv) > 3:
    for f in sys.argv[3:]:
        if f == "-f" or f == "--fix":
            fixMode = True;
            continue
        if fixMode:
            parser.setContentHandler(RouteReader(net, types, f + '.fixed'))
            print "fixing " + f
        else:
            print "checking " + f
        parser.parse(f)
else:
    parser.parse(sys.argv[2])
