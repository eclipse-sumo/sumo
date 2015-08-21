#!/usr/bin/env python
"""
@file    generateBidiDistricts.py
@author  Jakob Erdmann
@date    2015-07-31
@version $Id: route2poly.py 18096 2015-03-17 09:50:59Z behrisch $

Generate a taz (district) file which groups edges in opposite directions
belonging to the same road. For each edge, a taz is created which contains this edge
and its opposite.
This allows routing without the need for an
initial/final turn-around by replacing the attribute names 'from' and 'to' with
'fromTaz' and 'toTaz'

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import sys
import os
import itertools
from collections import defaultdict
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.output import parse
from sumolib.net import readNet
from sumolib.miscutils import Colorgen
from sumolib import geomhelper


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <netfile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    options, args = optParser.parse_args()
    try:
        options.net, = args
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.net + ".taz.xml"
    return options

DEBUGID = ["24214694","-24214694"]

def computeBidiTaz(net, radius=10):
    taz = defaultdict(set)
    for edge in net.getEdges():
        candidates = []
        r = min(radius, geomhelper.polyLength(edge.getShape()) / 2)
        if edge.getID() in DEBUGID: print r, edge.getLength(), edge.getShape()
        for x,y in edge.getShape():
            nearby = set()
            for edge2, dist in net.getNeighboringEdges(x, y, r):
                nearby.add(edge2)
            if edge.getID() in DEBUGID: print "  ",[e.getID() for e in nearby]
            candidates.append(nearby)
        opposites = reduce(lambda a,b:a.intersection(b), candidates)
        # XXX edges with the same endpoints should have roughly the same length
        # to be considered as opposites
        opposites.update(set(edge.getToNode().getOutgoing()).intersection(set(edge.getFromNode().getIncoming())))
        taz[edge.getID()] = opposites
    return taz


def main():
    options = parse_args()
    net = readNet(options.net)
    with open(options.outfile, 'w') as outf:
        outf.write('<tazs>\n')
        for tazID, edges in computeBidiTaz(net).items():
            outf.write('    <taz id="%s" edges="%s"/>\n' % (
                tazID, ' '.join(sorted([e.getID() for e in edges]))))
        outf.write('</tazs>\n')

if __name__ == "__main__":
    main()
