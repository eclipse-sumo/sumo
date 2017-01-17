#!/usr/bin/env python
"""
@file    route2trips.py
@author  Jakob Erdmann
@date    2015-08-05
@version $Id$

This script converts SUMO routes into an edge selection

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
import sys
from optparse import OptionParser
from sumolib.output import parse_fast


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <routefile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    options, args = optParser.parse_args()
    try:
        options.routefile, = args
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.routefile + ".sel.txt"
    return options


def main():
    options = parse_args()
    edges = set()

    for route in parse_fast(options.routefile, 'route', ['edges']):
        edges.update(route.edges.split())
    for walk in parse_fast(options.routefile, 'walk', ['edges']):
        edges.update(walk.edges.split())

    # warn about potentially missing edges
    for trip in parse_fast(options.routefile, 'trip', ['id', 'from', 'to']):
        edges.update([trip.attr_from, trip.to])
        print(
            "Warning: Trip %s is not guaranteed to be connected within the extracted edges." % trip.id)
    for walk in parse_fast(options.routefile, 'walk', ['from', 'to']):
        edges.update([walk.attr_from, walk.to])
        print("Warning: Walk from %s to %s is not guaranteed to be connected within the extracted edges." % (
            walk.attr_from, walk.to))

    with open(options.outfile, 'w') as outf:
        for e in sorted(list(edges)):
            outf.write('edge:%s\n' % e)

if __name__ == "__main__":
    main()
