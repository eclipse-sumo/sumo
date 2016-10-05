#!/usr/bin/env python
"""
@file    route_departOffset.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    11.09.2009
@version $Id$

Applies a given offset to the given route's departure time

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
import sys
import optparse
import array
from xml.sax import make_parser, handler


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-r", "--input-file", dest="infile", help="the input route file (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile", help="the output route file (mandatory)")
    optParser.add_option("-d", "--depart-offset", dest="offset", type="float", help="the depart offset to apply (mandatory)")
    optParser.add_option("--modify-ids", dest="modify_ids", action="store_true", default=False, help="whether ids should be modified as well")

    (options, args) = optParser.parse_args(args=args)
    if not options.infile or not options.outfile or not options.offset:
        optParser.print_help()
        sys.exit()

    if int(options.offset) == options.offset:
        options.offset = int(options.offset)

    return options


class RouteReader(handler.ContentHandler):

    def __init__(self, options, out):
        self._options = options
        self._out = out

    def startElement(self, name, attrs):
        self._out.write('<' + name)
        for a in attrs.keys():
            val = attrs[a]
            if a in ["depart", "begin", "end"]:
                val = str(int(val) + self._options.offset)
            if a == "id" and self._options.modify_ids:
                val = val + "_" + str(self._options.offset)
            self._out.write(' ' + a + '="' + val + '"')
        self._out.write('>')

    def endElement(self, name):
        self._out.write('</' + name + '>')

    def characters(self, content):
        self._out.write(content)


def main(options):
    with open(options.outfile, "w") as out:
        parser = make_parser()
        parser.setContentHandler(RouteReader(options, out))
        parser.parse(options.infile)

if __name__ == "__main__":
    main(get_options(sys.argv))

