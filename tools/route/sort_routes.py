#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    sort_routes.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Pieter Loof
# @date    2011-07-14

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from xml.dom import pulldom
from xml.sax import handler
from xml.sax import make_parser

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
from sumolib.options import ArgumentParser  # noqa
from sumolib.miscutils import parseTime  # noqa
import sumolib  # noqa

DEPART_ATTRS = {'vehicle': 'depart', 'trip': 'depart', 'flow': 'begin', 'person': 'depart', 'personFlow': 'begin'}
VEHICLE_TAGS = {'vehicle', 'trip', 'flow'}
DEFINITION_TAGS = {'vType', 'vTypeDistribution', 'route', 'routeDistribution'}
DEFINITION_DEPART = float("-inf")


def departure_key(depart, trigger, vehicle_departs):
    """Sort a triggered person flow after its starting vehicle."""
    if trigger in vehicle_departs:
        return vehicle_departs[trigger], True
    return depart, False


def get_options(args=None):
    ap = ArgumentParser()
    ap.add_argument("-o", "--outfile", category="output", type=ap.file, help="name of output file")
    ap.add_argument("-b", "--big", action="store_true", default=False,
                    help="Use alternative sorting strategy for large files (slower but more memory efficient)")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Tell me what you are doing")
    ap.add_argument("routefile", category="input", type=ap.file, help="route file whose routes should be sorted")
    options = ap.parse_args(args=args)
    if options.outfile is None:
        options.outfile = options.routefile + ".sorted"
    return options


def sort_departs(routefile, outfile, verbose):
    if isinstance(routefile, str):
        stream = open(routefile, 'rb')
    else:
        stream = routefile
        stream.seek(0)
    routes_doc = pulldom.parse(stream)
    vehicles = []
    vehicle_departs = {}
    root = None
    for event, parsenode in routes_doc:
        if event == pulldom.START_ELEMENT:
            if root is None:
                root = parsenode.localName
                sumolib.writeXMLHeader(outfile, root=root, includeXMLDeclaration=False)
                continue
            routes_doc.expandNode(parsenode)
            departAttr = DEPART_ATTRS.get(parsenode.localName)
            if departAttr is not None:
                startString = parsenode.getAttribute(departAttr)
                if departAttr == "begin" and not startString:
                    startString = "0"
                if startString == "triggered":
                    start = -1  # before everything else
                else:
                    start = parseTime(startString)
                trigger = None
                if parsenode.localName in VEHICLE_TAGS:
                    vehicle_departs[parsenode.getAttribute("id")] = start
                elif parsenode.localName == "personFlow" and startString == "triggered":
                    rides = parsenode.getElementsByTagName("ride")
                    if rides:
                        trigger = rides[0].getAttribute("lines")
                vehicles.append(
                    (start, trigger, parsenode.toprettyxml(indent="", newl="")))
            else:
                # copy to output
                outfile.write(
                    " " * 4 + parsenode.toprettyxml(indent="", newl="") + "\n")

    if verbose:
        print('read %s elements.' % len(vehicles))
    vehicles.sort(key=lambda v: departure_key(v[0], v[1], vehicle_departs))
    for _, _, vehiclexml in vehicles:
        outfile.write(" " * 4 + vehiclexml + "\n")
    outfile.write("</%s>\n" % root)
    if verbose:
        print('wrote %s elements.' % len(vehicles))
    if isinstance(routefile, str):
        stream.close()


class RouteHandler(handler.ContentHandler):

    def __init__(self, elements_with_depart):
        self.elements_with_depart = elements_with_depart
        self.vehicle_departs = {}
        self._depart = None
        self._trigger = None
        self._triggered_person_flow = False
        self._depth = 0

    def setDocumentLocator(self, locator):
        self.locator = locator

    def startElement(self, name, attrs):
        self._depth += 1
        if self._depth == 2:
            departAttr = DEPART_ATTRS.get(name)
            if departAttr is None:
                # Keep definitions such as vTypes before the sorted demand.
                if name in DEFINITION_TAGS:
                    self._depart = DEFINITION_DEPART
            else:
                startString = attrs.get(departAttr, "0") if departAttr == "begin" else attrs[departAttr]
                self._depart = -1 if startString == "triggered" else parseTime(startString)
                if name in VEHICLE_TAGS:
                    self.vehicle_departs[attrs["id"]] = self._depart
                self._triggered_person_flow = name == "personFlow" and startString == "triggered"
            self._start_line = self.locator.getLineNumber()
        elif self._depth == 3 and name == "ride":
            if self._triggered_person_flow and self._trigger is None:
                self._trigger = attrs.get("lines", "")
            elif self._depart == -1 and "depart" in attrs:
                # Put triggered persons behind their vehicle when vehroute output supplies the departure.
                self._depart = parseTime(attrs["depart"])

    def endElement(self, name):
        if self._depth == 2:
            if self._depart is not None:
                self.elements_with_depart.append(
                    (self._depart, self._trigger, self._start_line, self.locator.getLineNumber()))
            self._depart = None
            self._trigger = None
            self._triggered_person_flow = False
        self._depth -= 1


def create_line_index(file, verbose):
    if verbose:
        print("Building line offset index for %s" % file)
    result = []
    offset = 0
    with open(file, 'rb') as f:  # need to read binary here for correct offsets
        for line in f:
            result.append(offset)
            offset += len(line)
    return result


def get_element_lines(routefilename, verbose):
    # [(departure, trigger vehicle ID, first line, last line), ...]
    if verbose:
        print("Parsing %s for line indices and departs" % routefilename)
    result = []
    parser = make_parser()
    route_handler = RouteHandler(result)
    parser.setContentHandler(route_handler)
    parser.parse(open(routefilename))
    if verbose:
        print("  found %s items" % sum(depart != DEFINITION_DEPART for depart, _, _, _ in result))
    result.sort(key=lambda v: departure_key(v[0], v[1], route_handler.vehicle_departs))
    return result


def copy_elements(routefilename, outfilename, element_lines, line_offsets, verbose):
    if verbose:
        print("Copying elements from %s to %s sorted by departure" % (
            routefilename, outfilename))
    # don't read binary here for line end conversion
    with open(routefilename) as routefile, open(outfilename, 'w') as outfile:
        # copy header
        for line in routefile:
            outfile.write(line)
            # find start of the route file but ignore option in header comment
            if '<routes' in line and 'value=' not in line:
                break
        for _, _, start, end in element_lines:
            # convert from 1-based to 0-based indices
            routefile.seek(line_offsets[start - 1])
            for __ in range(end - start + 1):
                outfile.write(routefile.readline())
        outfile.write('</routes>')


def main(args=None):
    options = get_options(args=args)
    if options.big:
        line_offsets = create_line_index(options.routefile, options.verbose)
        element_lines = get_element_lines(options.routefile, options.verbose)
        copy_elements(options.routefile, options.outfile, element_lines, line_offsets, options.verbose)
    else:
        with open(options.routefile) as routefile, open(options.outfile, 'w') as outfile:
            # copy header
            for line in routefile:
                if line.find('<routes') == 0 or line.find('<additional') == 0:
                    break
                outfile.write(line)
            sort_departs(routefile, outfile, options.verbose)


if __name__ == "__main__":
    main()
