#!/usr/bin/env python
"""
@file    findUTurns.py
@author  Michael.Behrisch@dlr.de
@date    2008-04-16
@version $Id$

Determines the number of U turns in a route file by comparing 
the edge ids of successive edges for "negation". This works with
most Elmar (Navteq) imported nets. Optional it can also remove
U turns at the start and the end of a route and output the 
modified routes.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, sys

from xml.sax import make_parser, handler
from optparse import OptionParser

class RouteReader(handler.ContentHandler):

    def __init__(self):
        self._vehAttrs = None
        self._routeString = ''
        self._routeCount = 0
        self._uTurnCount = 0
        self._uTurnFirst = 0
        self._uTurnLast = 0
        self._short = 0

    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vehAttrs = attrs
        elif name == 'route':
            self._routeString = ''
            if attrs.has_key("edges"):
                self._routeString = attrs['edges']
        elif name == 'routes':
            if options.repair:
                print '<routes>'
            print >> sys.stderr, "   Route  UTurn UTurnFirst UTurnLast  Short"
        else:
            if options.repair:
                print '<' + name,
                for key in attrs.keys():
                    print '%s="%s"' % (key, attrs[key]),
                print '/>'

    def characters(self, content):
        self._routeString += content

    def endElement(self, name):
        if name == 'route':
            self._routeCount += 1
            lastEdge = ' '
            route = self._routeString.split()
            routeStart = 0
            routeEnd = len(route)
            if len(route) == 2:
                self._short += 1
            for index, edge in enumerate(route):
                if (lastEdge[0] == '-' and edge == lastEdge[1:]) or\
                   (edge[0] == '-' and lastEdge == edge[1:]):
                    self._uTurnCount += 1
                    if index == 1:
                        self._uTurnFirst += 1
                        routeStart = 1
                    if index == len(route) - 1:
                        self._uTurnLast += 1
                        routeEnd -= 1
                lastEdge = edge
            if self._routeCount % 10000 == 0:
                print >> sys.stderr, "%8i %6i %10i %9i %6i"\
                                     % (self._routeCount, self._uTurnCount,
                                        self._uTurnFirst, self._uTurnLast,
                                        self._short), "\r",
            if routeEnd - routeStart > 1:
                if options.repair:
                    print '<vehicle',
                    for key in self._vehAttrs.keys():
                        print '%s="%s"' % (key, self._vehAttrs[key]),
                    print '>'
                    print '   <route edges="%s"/>' % ' '.join(route[routeStart:routeEnd])
                    print '</vehicle>'
        elif name == 'routes':
            if options.repair:
                print '</routes>'
            print >> sys.stderr, "%8i %6i %10i %9i %6i"\
                                 % (self._routeCount, self._uTurnCount,
                                    self._uTurnFirst, self._uTurnLast,
                                    self._short)

optParser = OptionParser(usage="usage: %prog [options] <routefile>+")
optParser.add_option("-r", "--repair", action="store_true", dest="repair",
                     default=False, help="remove U turns at start and end of the route")
(options, args) = optParser.parse_args()
if len(args) == 0:
    optParser.error("Please give at least one route file!")
parser = make_parser()
for f in args:
    parser.setContentHandler(RouteReader())
    parser.parse(f)
