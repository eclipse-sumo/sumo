#!/usr/bin/python
"""
@file    routecheck.py
@author  Michael.Behrisch@dlr.de
@date    2007-03-09
@version $Id: routecheck.py 6623 2009-01-15 10:35:15Z behrisch $

This script does simple checks for the routes on a given network.
It needs at least two parameters, the SUMO net (.net.xml) and a file
specifying routes.
Warnings will be issued if there is an unknown edge in the route,
if the route is too short (only one edge), if the route is disconnected
or if the route definition does not use the edges attribute.
If one specifies -f or --fix all subsequent route files will be fixed
(if possible). At the moment this means adding an intermediate edge
if just one link is missing in a disconnected route, or adding an edges
attribute if it is missing.
All changes are documented within the output file which has the same name
as the input file with an additional .fixed suffix.

Copyright (C) 2007-2009 DLR/TS, Germany
All rights reserved
"""
import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edge = ''
        self._nb = {}

    def startElement(self, name, attrs):
        if name == 'edge' and (not attrs.has_key('function') or attrs['function'] != 'internal'):
            self._edge = attrs['id']
            self._nb[self._edge] = set()
        elif name == 'succ':
            self._edge = attrs['edge']
        elif name == 'succlane':
            if self._edge in self._nb:
                l = attrs['lane']
                if l != "SUMO_NO_DESTINATION":
                    self._nb[self._edge].add(l[:l.rfind('_')])

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

            
class RouteReader(handler.ContentHandler):

    def __init__(self, net, outFileName):
        self._vID = ''
        self._routeID = ''
        self._routeString = ''
        self._addedString = ''
        self._net = net
        if outFileName:
            self._out = open(outFileName, 'w')
        else:
            self._out = None
        self._fileOut = None
        self._isRouteValid = True
        self._changed = False
        
    def startDocument(self):
        if self._out:
            print >> self._out, '<?xml version="1.0"?>'

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
            if self._addedString != '':
                self._fileOut.write("<!-- added edges: " + self._addedString + "-->")
                self._addedString = ''
            self._out.close()
            self._out = self._fileOut
            self._fileOut = None

    def startElement(self, name, attrs):
        if name == 'vehicle' and not attrs.has_key('route'):
            self.condOutputRedirect()
            self._vID = attrs['id']
        if name == 'route':
            self.condOutputRedirect()
            if attrs.has_key('id'):
                self._routeID = attrs['id']
            else:
                self._routeID = "for vehicle " + self._vID
            self._routeString = ''
            if attrs.has_key('edges'):
                self._routeString = attrs['edges']
            else:
                self._changed = True
                print "Warning: No edges attribute in route " + self._routeID
        elif self._routeID:
            print "Warning: This script does not handle nested '%s' elements properly." % name
        if self._out:
            self._out.write('<' + name)
            for (key, value) in attrs.items():
                if name != 'route' or key != "edges":
                    self._out.write(' %s="%s"' % (key, saxutils.escape(value)))
            if name != 'route':
                self._out.write('>')

    def endElement(self, name):
        if name == 'route':
            self._isRouteValid = self.testRoute()
            if self._out:
                self._out.write(' edges="%s"/>' % self._routeString)
            self._routeID = ''
            self._routeString = ''
            if self._vID == '':
                self.endOutputRedirect()
        elif name == 'vehicle' and self._vID != '':
            self._vID = ''
            if self._out:
                self._out.write('</vehicle>')
            self.endOutputRedirect()
        elif self._out:
            self._out.write('</%s>' % name)

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
                print "This is no problem if your sumo version is at least 0.10.0"
            doConnectivityTest = True
            for v in rdata:
                if not self._net.hasEdge(v):
                    print "Warning: Unknown edge " + v + " in route " + self._routeID
                    returnValue = False
                    doConnectivityTest = False
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
            return returnValue
        return False
                    
    def ignorableWhitespace(self, content):
        if self._out:
            self._out.write(content)
        
    def processingInstruction(self, target, data):
        if self._out:
            self._out.write('<?%s %s?>' % (target, data))


            
if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <net> <routes>* [-f|--fix] <routes>*"
    sys.exit()
parser = make_parser()
net = NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
fixMode = False
parser.setContentHandler(RouteReader(net, ''))
for f in sys.argv[2:]:
    if f == "-f" or f == "--fix":
        fixMode = True;
        continue
    if fixMode:
        parser.setContentHandler(RouteReader(net, f + '.fixed'))
        print "fixing " + f
    else:
        print "checking " + f
    parser.parse(f)
