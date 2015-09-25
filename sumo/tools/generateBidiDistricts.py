#!/usr/bin/env python
"""
@file    generateBidiDistricts.py
@author  Jakob Erdmann
@date    2015-07-31
@version $Id$

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
from optparse import OptionParser

from sumolib.net import readNet
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


def computeBidiTaz(net, radius=10):
    for edge in net.getEdges():
        candidates = []
        r = min(radius, geomhelper.polyLength(edge.getShape()) / 2)
        for x, y in edge.getShape():
            nearby = set()
            for edge2, dist in net.getNeighboringEdges(x, y, r):
                nearby.add(edge2)
            candidates.append(nearby)
        opposites = reduce(lambda a, b: a.intersection(b), candidates)
        opposites.update(set(edge.getToNode().getOutgoing()).intersection(
            set(edge.getFromNode().getIncoming())))
        yield edge, opposites


def main(netFile, outFile):
    net = readNet(netFile, withConnections=False, withFoes=False)
    with open(outFile, 'w') as outf:
        outf.write('<tazs>\n')
        for taz, edges in computeBidiTaz(net):
            outf.write('    <taz id="%s" edges="%s"/>\n' % (
                taz.getID(), ' '.join(sorted([e.getID() for e in edges]))))
        outf.write('</tazs>\n')
    return net

if __name__ == "__main__":
    options = parse_args()
    main(options.net, options.outfile)
