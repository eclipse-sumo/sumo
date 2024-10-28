#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    route2alts.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    11.09.2009

"""
Counts possible routes for all depart/arrival edges.
Builds route alternatives assigning the so determined probabilities to use a route.

Please note that the cost of the route is not computed!
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os
from xml.sax import make_parser, handler

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.options import ArgumentParser
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    optParser = ArgumentParser()
    optParser.add_argument("input", category='input', help="Provide an input network")
    optParser.add_argument("output", category='output', help="Provide an output name")
    return optParser.parse_args(args=args)


class RouteCounter(handler.ContentHandler):

    def __init__(self):
        self._routeCounts = {}
        self._odCounts = {}
        self._odRoutes = {}

    def startElement(self, name, attrs):
        if "edges" in attrs:
            route = attrs["edges"]
            edges = route.split(" ")
            od = (edges[0], edges[-1])
            # count od occurrences
            if od in self._odCounts:
                self._odCounts[od] = self._odCounts[od] + 1
            else:
                self._odCounts[od] = 1
            # count route occurrences
            if route in self._routeCounts:
                self._routeCounts[route] = self._routeCounts[route] + 1
            else:
                self._routeCounts[route] = 1
            # build map od->routes
            if od not in self._odRoutes:
                self._odRoutes[od] = []
            if route not in self._odRoutes[od]:
                self._odRoutes[od].append(route)

    def endDocument(self):
        self._odRouteProbs = {}
        for od in self._odCounts:
            self._odRouteProbs[od] = {}
            absNo = float(self._odCounts[od])
            for route in self._odRoutes[od]:
                self._odRouteProbs[od][route] = float(
                    self._routeCounts[route]) / absNo


class RoutePatcher(handler.ContentHandler):

    def __init__(self, stats, out):
        self._stats = stats
        self._out = out

    def startElement(self, name, attrs):
        if name != "route":
            self._out.write('<' + name)
            for a in attrs.keys():
                self._out.write(' ' + a + '="' + attrs[a] + '"')
            self._out.write('>')
        else:
            self._out.write('\n        <routeDistribution last="0">\n')
            route = attrs["edges"]
            edges = route.split(" ")
            od = (edges[0], edges[-1])
            self._out.write('            <route cost="1" probability="' + str(
                self._stats._odRouteProbs[od][route]) + '" edges="' + route + '"/>\n')
            for r in self._stats._odRouteProbs[od]:
                if r == route:
                    continue
                else:
                    self._out.write('            <route cost="1" probability="' + str(
                        self._stats._odRouteProbs[od][r]) + '" edges="' + r + '"/>\n')
            self._out.write('        </routeDistribution>\n    ')

    def endElement(self, name):
        if name != "route":
            self._out.write('</' + name + '>')

    def characters(self, content):
        self._out.write(content)


def main(options):
    # if len(sys.argv) < 3:
    #     print("Usage: route2alts.py <INPUT_FILE> <OUTPUT_FILE>")
    #     sys.exit()
    # count occurrences
    print("Counting alternative occurrences...")
    parser = make_parser()
    counter = RouteCounter()
    parser.setContentHandler(counter)
    parser.parse(options.input)
    # build alternatives
    print("Building alternatives...")
    with open(options.output, "w") as out:
        parser = make_parser()
        parser.setContentHandler(RoutePatcher(counter, out))
        parser.parse(options.input)


if __name__ == "__main__":
    main(get_options())
