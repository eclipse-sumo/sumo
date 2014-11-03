#!/usr/bin/env python
"""
@file    route2trips.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2008-03-19
@version $Id$

This script converts SUMO routes back into SUMO trips which serve
as input to one of the routing applications.
It reads the routes from a file given as first parameter
and outputs the trips to stdout.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import sys, datetime

from xml.sax import parse, handler

class RouteReader(handler.ContentHandler):

    def __init__(self, attrList):
        self._vType = ''
        self._vID = ''
        self._vDepart = 0
        self._routeID = ''
        self._routeString = ''
        self._routes = {}
        self._attrList = attrList
        self._vehicleAttrs = None
        
    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vehicleAttrs = dict(attrs)
            self._vID = attrs['id']
            if attrs.has_key('route'):
                self._routeString = self._routes[attrs['route']]
                del self._vehicleAttrs['route']
        elif name == 'route':
            if not self._vID:
                self._routeID = attrs['id']
            self._routeString = ''
            if attrs.has_key('edges'):
                self._routeString = attrs['edges']
        elif name == 'routes':
            print("""<?xml version="1.0"?>
<!-- generated on %s by $Id$ -->
<trips>""" % datetime.datetime.now())

    def endElement(self, name):
        if name == 'route':
            if not self._vID:
                self._routes[self._routeID] = self._routeString
                self._routeString = ''
            self._routeID = ''
        elif name == 'vehicle':
            edges = self._routeString.split()
            self._vehicleAttrs["from"] = edges[0]
            self._vehicleAttrs["to"] = edges[-1]
            if self._attrList:
                print('    <trip %s/>' % (' '.join(['%s="%s"' % (key, self._vehicleAttrs[key]) for key in self._attrList])))
            else:
                del self._vehicleAttrs['id']
                items = sorted(['%s="%s"' % (key, val) for key, val in self._vehicleAttrs.iteritems()])
                print('    <trip id="%s" %s/>' % (self._vID, ' '.join(items)))
            self._vID = ''
            self._routeString = ''
        elif name == 'routes':
            print("</trips>")

    def characters(self, content):
        self._routeString += content


if len(sys.argv) < 2:
    sys.exit("Usage: " + sys.argv[0] + " <routes> [<attribute>*]")
parse(sys.argv[1], RouteReader(sys.argv[2:]))
