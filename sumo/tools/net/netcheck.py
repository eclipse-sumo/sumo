#!/usr/bin/env python
"""
@file    netcheck.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Jakob Erdmann
@date    2007-03-20
@version $Id$

This script does simple check for the network.
It tests whether the network is (weakly) connected.
It needs one parameter, the SUMO net (.net.xml).

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2007-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os
import sys
from optparse import OptionParser
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <net> <options>"
    optParser = OptionParser()
    optParser.add_option("-s", "--source",
                         default=False, help="List edges reachable from the source")
    optParser.add_option("-d", "--destination",
                         default=False, help="List edges which can reach the destination")
    optParser.add_option("-o", "--selection-output",
                         help="Write output to file(s) as a loadable selection")
    optParser.add_option("-l", "--vclass", help="Include only edges allowing VCLASS")

    options, args = optParser.parse_args()
    if len(args) != 1:
        sys.exit(USAGE)
    options.net = args[0]
    return options


def getWeaklyConnected(net, vclass=None):
    components = []
    edgesLeft = set(net.getEdges())
    queue = list()
    while len(edgesLeft) != 0:
        component = set()
        queue.append(edgesLeft.pop())
        while not len(queue) == 0:
            edge = queue.pop(0)
            if vclass is None or edge.allows(vclass): 
                component.add(edge.getID())
                for n in edge.getOutgoing().iterkeys():
                    if n in edgesLeft:
                        queue.append(n)
                        edgesLeft.remove(n)
                for n in edge.getIncoming().iterkeys():
                    if n in edgesLeft:
                        queue.append(n)
                        edgesLeft.remove(n)
        if component:
            components.append(component)
    return components


def getReachable(net, source_id, options, useIncoming=False):
    if not net.hasEdge(source_id):
        sys.exit("'%s' is not a valid edge id" % source_id)
    source = net.getEdge(source_id)
    if options.vclass is not None and not source.allows(options.vclass): 
        sys.exit("'%s' does not allow %s" % (source_id, options.vclass))
    fringe = [source]
    found = set()
    found.add(source)
    while len(fringe) > 0:
        new_fringe = []
        for edge in fringe:
            cands = edge.getIncoming() if useIncoming else edge.getOutgoing()
            for reachable in cands.iterkeys():
                if options.vclass is None or reachable.allows(options.vclass): 
                    if not reachable in found:
                        found.add(reachable)
                        new_fringe.append(reachable)
        fringe = new_fringe

    if useIncoming:
        print "%s of %s edges can reach edge '%s':" % (
            len(found), len(net.getEdges()), source_id)
    else:
        print "%s of %s edges are reachable from edge '%s':" % (
            len(found), len(net.getEdges()), source_id)

    if options.selection_output:
        with open(options.selection_output, 'w') as f:
            for e in found:
                f.write("edge:%s\n" % e.getID())
    else:
        print [e.getID() for e in found]


if __name__ == "__main__":
    options = parse_args()

    net = sumolib.net.readNet(options.net)
    if options.source:
        getReachable(net, options.source, options)
    elif options.destination:
        getReachable(net, options.destination, options, True)
    else:
        components = getWeaklyConnected(net, options.vclass)
        if len(components) != 1:
            print "Warning! Net is not connected."
            for idx, comp in enumerate(sorted(components, key=lambda c: iter(c).next())):
                if options.selection_output:
                    with open(options.selection_output + "comp%s.txt" % idx, 'w') as f:
                        for e in comp:
                            f.write("edge:%s\n" % e)
                else:
                    print "Component", idx
                    print " ".join(comp)
                    print
