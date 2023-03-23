#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateRerouters.py
# @author  Jakob Erdmann
# @date    2023-02-07

"""
This script generates rerouters for a given list of closed edges and
automatically finds upstream rerouter edges that permit rerouting.
"""
from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Generate rerouter definition for closed edges",)
    op.add_option("-n", "--net-file", category="input", dest="netfile",
                  help="define the net file (mandatory)")
    op.add_option("-o", "--output-file", category="output", dest="outfile", default="rerouters.xml",
                  help="define the output rerouter filename")
    op.add_option("-x", "--closed-edges", category="input", dest="closedEdges",
                  help="provide a comma-separated list of edges to close")
    op.add_option("-i", "--id-prefix", category="processing", dest="idPrefix", default="rr",
                  help="id prefix for generated rerouters")
    op.add_option("--vclass", category="processing", default="passenger",
                  help="only consider necessary detours for the given vehicle class (default passenger)")
    op.add_option("--allow", category="processing", default="authority",
                  help="vClasses that shall be permitted on the closed edge")
    op.add_option("--disallow", category="processing",
                  help="vClasses that shall be prohibited on the closed edge")
    op.add_option("-b", "--begin", category="time", default=0, type=float,
                  help="begin time for the closing")
    op.add_option("-e", "--end", category="time", default=86400, type=float,
                  help="end time for the closing (default 86400)")
    options = op.parse_args(args=args)
    if not options.netfile or not options.closedEdges:
        op.print_help()
        sys.exit(1)

    options.closedEdges = options.closedEdges.split(',')
    return options


def findNotifcationEdges(options, net, closedEdges):
    result = set()

    # close edges in the network
    for e in closedEdges:
        for lane in e.getLanes():
            p = set(lane.getPermissions())
            p.remove(options.vclass)
            lane.setPermissions(p)

    for e in closedEdges:
        reachable = set()
        for succ in e.getOutgoing().keys():
            if succ.allows(options.vclass):
                reachable.update(net.getReachable(succ, options.vclass))

        upstream = []
        seen = set()
        for pred in e.getIncoming().keys():
            if pred.allows(options.vclass):
                upstream.append(pred)

        while upstream and reachable:
            cand = upstream.pop(0)
            seen.add(cand)
            reachable2 = net.getReachable(cand, options.vclass)
            found = reachable2.intersection(reachable)
            if found:
                result.add(cand)
                reachable.difference_update(found)
            for pred in cand.getIncoming().keys():
                if pred.allows(options.vclass):
                    if pred not in seen:
                        upstream.append(pred)
    return result


def main(options):
    net = sumolib.net.readNet(options.netfile)

    closedEdges = []
    for closedID in options.closedEdges:
        if not net.hasEdge(closedID):
            sys.exit("Unknown closed edge '%s'" % closedID)
        closedEdges.append(net.getEdge(closedID))

    allowDisallow = ""
    if options.disallow is not None:
        allowDisallow = ' disallow="%s"' % options.disallow
    elif options.allow != "":
        allowDisallow = ' allow="%s"' % options.allow

    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)

        rerouterEdges = findNotifcationEdges(options, net, closedEdges)
        rerouterEdgeIDs = sorted([e.getID() for e in rerouterEdges])

        outf.write('   <rerouter id="%s" edges="%s">\n' % (
                   options.idPrefix,
                   ' '.join(rerouterEdgeIDs)))
        outf.write('      <interval begin="%s" end="%s">\n' % (options.begin, options.end))
        for e in closedEdges:
            outf.write('          <closingReroute id="%s"%s>\n' % (e.getID(), allowDisallow))

        outf.write('       </interval>\n')
        outf.write('   </rerouter>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    main(get_options())
