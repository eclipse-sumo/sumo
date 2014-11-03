#!/usr/bin/env python
"""
@file    sort_routes.py
@author  Jakob Erdmann
@author  Michael Behrisch
@author  Pieter Loof
@date    2011-07-14
@version $Id$

This script sorts the vehicles in the given route file by their depart time
SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2007-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import sys
import os
import re
from xml.dom import pulldom
from xml.sax import handler
from xml.sax import make_parser
from xml.sax import handler
from optparse import OptionParser

def get_options(args=None):
    USAGE = "Usage: " + sys.argv[0] + " <routefile>"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    optParser.add_option("-b", "--big", action="store_true", default=False, 
            help="Use alternative sortign strategy for large files (slower but more memory efficient)")
    options, args = optParser.parse_args(args=args)
    if len(args) != 1:
        sys.exit(USAGE)
    options.routefile = args[0]
    if options.outfile is None:
        options.outfile = options.routefile + ".sorted"
    return options 


def sort_departs(routefilename, outfile):
    routes_doc = pulldom.parse(sys.argv[1])
    vehicles = []
    for event, parsenode in routes_doc:
        if event == pulldom.START_ELEMENT and (parsenode.localName == 'vehicle' or parsenode.localName == 'flow'):
            vehicle = parsenode # now we know it's a vehicle or a flow
            routes_doc.expandNode(vehicle)
            if (parsenode.localName == 'vehicle'):
                depart = int(float(vehicle.getAttribute('depart')))
                vehicles.append((depart, vehicle.toprettyxml(indent="", newl="")))
            elif (parsenode.localName == 'flow'):
                begin = int(float(vehicle.getAttribute('begin')))
                vehicles.append((begin, vehicle.toprettyxml(indent="", newl="")))
    print('read %s elements.' % len(vehicles))
    vehicles.sort()
    for depart, vehiclexml in vehicles:
        outfile.write(" "*4)
        outfile.write(vehiclexml)
        outfile.write("\n")
    print('wrote %s elements.' % len(vehicles))


class RouteHandler(handler.ContentHandler):
    def __init__(self, elements_with_depart):
        self.DEPART_ATTR = {'vehicle' : 'depart', 'flow' : 'begin'}
        self.elements_with_depart = elements_with_depart
        self._depart = None

    def setDocumentLocator(self,locator):
        self.locator = locator

    def startElement(self,name,attrs):
        if name in self.DEPART_ATTR.keys():
            self._depart = attrs[self.DEPART_ATTR[name]]
            self._start_line = self.locator.getLineNumber()

    def endElement(self,name):
        if name in self.DEPART_ATTR.keys():
            end_line = self.locator.getLineNumber()
            self.elements_with_depart.append((self._depart, self._start_line, end_line))


def create_line_index(file):
    print "Building line offset index for %s" % file
    result = []
    offset = 0
    with open(file, 'rb') as f: # need to read binary here for correct offsets
        for line in f:
            result.append(offset)
            offset += len(line)
    return result


def get_element_lines(routefilename):
    # [(depart, line_index_where_element_starts, line_index_where_element_ends), ...]
    print "Parsing %s for line indices and departs" % routefilename
    result = []
    parser = make_parser()
    parser.setContentHandler(RouteHandler(result))
    parser.parse(open(routefilename))
    print "  found %s items" % len(result)
    return result


def copy_elements(routefilename, outfilename, element_lines, line_offsets):
    print "Copying elements from %s to %s sorted by departure" % (
            routefilename, outfilename)
    outfile = open(outfilename, 'w')
    # copy header
    for line in open(routefilename):
        outfile.write(line)
        if '<routes' in line:
            break
    with open(routefilename) as f: # don't read binary here for line end conversion
        for depart, start, end in element_lines:
            # convert from 1-based to 0-based indices
            f.seek(line_offsets[start - 1])
            for i in range(end - start + 1):
                outfile.write(f.readline())
    outfile.write('</routes>')
    outfile.close()


def main(args=None):
    options = get_options(args=args)
    if options.big:
        line_offsets = create_line_index(options.routefile)
        element_lines = get_element_lines(options.routefile)
        element_lines.sort()
        copy_elements(options.routefile, options.outfile, element_lines, line_offsets)
    else:
        outfile = open(options.outfile, 'w')
        close_line = ''
        for line in open(options.routefile):
            if '<routes' in line:
                close_line = '</routes>'
            if '<additional' in line:
                close_line = '</additional>'
            if '<vehicle ' in line or '<flow ' in line:
                break
            outfile.write(line)
        sort_departs(options.routefile, outfile)
        outfile.write(close_line)
        outfile.close()


if __name__ == "__main__":
    main()
