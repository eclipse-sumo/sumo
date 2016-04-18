#!/usr/bin/env python
"""
@file    findUTurns.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2008-04-16
@version $Id$

Determines the number of U turns in a route file by comparing 
the edge ids of successive edges for "negation". This works wherever the sumo network
was imported from a data source which uses bidirectional street definitions
(OSM, NavTeq). In this case the unidirectional edges receive the original ID in
the forward direction and a new ID composed of the prefix '-' and the original
ID in the reverse direction by convention.

XXX repair is not working. DUAROUTER with --remove-loops should be used for that purpose

Optional this script can also remove
U turns at the start and the end of a route and output the 
modified routes.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys

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
            if "edges" in attrs:
                self._routeString = attrs['edges']
        elif name == 'routes':
            if options.repair:
                print('<routes>')
            print(
                "   Route  UTurn UTurnFirst UTurnLast  Short", file=sys.stderr)
        else:
            if options.repair:
                print('<' + name, end=' ')
                for key in attrs.keys():
                    print('%s="%s"' % (key, attrs[key]), end=' ')
                print('/>')

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
                print("%8i %6i %10i %9i %6i"
                      % (self._routeCount, self._uTurnCount,
                         self._uTurnFirst, self._uTurnLast,
                         self._short), "\r", end=' ', file=sys.stderr)
            if routeEnd - routeStart > 1:
                if options.repair:
                    print('<vehicle', end=' ')
                    for key in self._vehAttrs.keys():
                        print('%s="%s"' % (key, self._vehAttrs[key]), end=' ')
                    print('>')
                    print('   <route edges="%s"/>' %
                          ' '.join(route[routeStart:routeEnd]))
                    print('</vehicle>')
        elif name == 'routes':
            if options.repair:
                print('</routes>')
            print("%8i %6i %10i %9i %6i"
                  % (self._routeCount, self._uTurnCount,
                     self._uTurnFirst, self._uTurnLast,
                     self._short), file=sys.stderr)

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
