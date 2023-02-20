#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
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
from optparse import OptionParser

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib  # noqa
from sumolib.miscutils import parseTime  # noqa

DEPART_ATTRS = {'vehicle': 'depart', 'trip': 'depart', 'flow': 'begin', 'person': 'depart'}


def get_options(args=None):
    USAGE = "Usage: " + sys.argv[0] + " <routefile>"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    optParser.add_option("-b", "--big", action="store_true", default=False,
                         help="Use alternative sorting strategy for large files (slower but more memory efficient)")
    options, args = optParser.parse_args(args=args)
    if len(args) != 1:
        sys.exit(USAGE)
    options.routefile = args[0]
    if options.outfile is None:
        options.outfile = options.routefile + ".sorted"
    return options


def sort_departs(routefile, outfile):
    if isinstance(routefile, str):
        stream = open(routefile, 'rb')
    else:
        stream = routefile
        stream.seek(0)
    routes_doc = pulldom.parse(stream)
    vehicles = []
    root = None
    for event, parsenode in routes_doc:
        if event == pulldom.START_ELEMENT:
            if root is None:
                root = parsenode.localName
                outfile.write("<%s>\n" % root)
                continue
            routes_doc.expandNode(parsenode)
            departAttr = DEPART_ATTRS.get(parsenode.localName)
            if departAttr is not None:
                startString = parsenode.getAttribute(departAttr)
                if startString == "triggered":
                    start = -1  # before everything else
                else:
                    start = parseTime(startString)
                vehicles.append(
                    (start, parsenode.toprettyxml(indent="", newl="")))
            else:
                # copy to output
                outfile.write(
                    " " * 4 + parsenode.toprettyxml(indent="", newl="") + "\n")

    print('read %s elements.' % len(vehicles))
    vehicles.sort(key=lambda v: v[0])
    for _, vehiclexml in vehicles:
        outfile.write(" " * 4 + vehiclexml + "\n")
    outfile.write("</%s>\n" % root)
    print('wrote %s elements.' % len(vehicles))
    if isinstance(routefile, str):
        stream.close()


class RouteHandler(handler.ContentHandler):

    def __init__(self, elements_with_depart):
        self.elements_with_depart = elements_with_depart
        self._depart = None

    def setDocumentLocator(self, locator):
        self.locator = locator

    def startElement(self, name, attrs):
        if name in DEPART_ATTRS.keys():
            startString = attrs[DEPART_ATTRS[name]]
            if startString == "triggered":
                self._depart = -1
            else:
                self._depart = parseTime(startString)
            self._start_line = self.locator.getLineNumber()
        if name == "ride" and self._depart == -1 and "depart" in attrs:
            # this is at least the attempt to put triggered persons behind their vehicle
            # it probably works only for vehroute output
            self._depart = parseTime(attrs["depart"])

    def endElement(self, name):
        if name in DEPART_ATTRS.keys():
            end_line = self.locator.getLineNumber()
            self.elements_with_depart.append(
                (self._depart, self._start_line, end_line))
            self._depart = None


def create_line_index(file):
    print("Building line offset index for %s" % file)
    result = []
    offset = 0
    with open(file, 'rb') as f:  # need to read binary here for correct offsets
        for line in f:
            result.append(offset)
            offset += len(line)
    return result


def get_element_lines(routefilename):
    # [(depart, line_index_where_element_starts, line_index_where_element_ends), ...]
    print("Parsing %s for line indices and departs" % routefilename)
    result = []
    parser = make_parser()
    parser.setContentHandler(RouteHandler(result))
    parser.parse(open(routefilename))
    print("  found %s items" % len(result))
    return result


def copy_elements(routefilename, outfilename, element_lines, line_offsets):
    print("Copying elements from %s to %s sorted by departure" % (
        routefilename, outfilename))
    # don't read binary here for line end conversion
    with open(routefilename) as routefile, open(outfilename, 'w') as outfile:
        # copy header
        for line in routefile:
            outfile.write(line)
            if '<routes' in line:
                break
        for _, start, end in element_lines:
            # convert from 1-based to 0-based indices
            routefile.seek(line_offsets[start - 1])
            for __ in range(end - start + 1):
                outfile.write(routefile.readline())
        outfile.write('</routes>')


def main(args=None):
    options = get_options(args=args)
    if options.big:
        line_offsets = create_line_index(options.routefile)
        element_lines = sorted(get_element_lines(options.routefile))
        copy_elements(options.routefile, options.outfile, element_lines, line_offsets)
    else:
        with open(options.routefile) as routefile, open(options.outfile, 'w') as outfile:
            # copy header
            for line in routefile:
                if line.find('<routes') == 0 or line.find('<additional') == 0:
                    break
                outfile.write(line)
            sort_departs(routefile, outfile)


if __name__ == "__main__":
    main()
