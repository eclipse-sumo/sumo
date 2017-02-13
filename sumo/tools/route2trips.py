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
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import datetime

from xml.sax import parse, handler

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib


class RouteReader(handler.ContentHandler):

    def __init__(self, attrList, outfile, vias, calledBy=""):
        self._vType = ''
        self._vID = ''
        self._vDepart = 0
        self._routeID = ''
        self._routeString = ''
        self._routes = {}
        self._attrList = attrList
        self._vehicleAttrs = None
        self.outfile = outfile
        self.vias = vias
        self.calledBy = calledBy

    def startElement(self, name, attrs):
        if name == 'vehicle':
            self._vehicleAttrs = dict(attrs)
            self._vID = attrs['id']
            if 'route' in attrs:
                self._routeString = self._routes[attrs['route']]
                del self._vehicleAttrs['route']
        elif name == 'route':
            if not self._vID:
                self._routeID = attrs['id']
            self._routeString = ''
            if 'edges' in attrs:
                self._routeString = attrs['edges']
        elif name == 'vType':
            # XXX does not handle child elements
            print('    <vType %s/>' % (' '.join(['%s="%s"' % (key, value) for key, value in sorted(dict(attrs).items())])),
                  file=self.outfile)
        elif name == 'routes':
            sumolib.writeXMLHeader(
                self.outfile,
                "$Id$%s" % self.calledBy, "routes")

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
            via = self.vias.get(self._vID, "")
            if self._attrList:
                print('    <trip %s%s/>' % (' '.join(['%s="%s"' % (key,
                                                                   self._vehicleAttrs[key]) for key in self._attrList]), via),
                      file=self.outfile)
            else:
                del self._vehicleAttrs['id']
                items = sorted(['%s="%s"' % (key, val)
                                for key, val in self._vehicleAttrs.items()])
                print('    <trip id="%s" %s%s/>' % (self._vID, ' '.join(items), via),
                      file=self.outfile)
            self._vID = ''
            self._routeString = ''
        elif name == 'routes':
            print("</routes>", file=self.outfile)

    def characters(self, content):
        self._routeString += content


def main(argv, outfile=None, vias={}, calledBy=""):
    routefile = argv[0]
    attrList = argv[1:]
    if outfile is None:
        parse(routefile, RouteReader(attrList, sys.stdout, vias, calledBy))
    else:
        with open(outfile, 'w') as outf:
            parse(routefile, RouteReader(attrList, outf, vias, calledBy))


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Usage: " + sys.argv[0] + " <routes> [<attribute>*]")
    main(sys.argv[1:])
