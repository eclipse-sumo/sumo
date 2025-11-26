#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
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
    op.add_option("-n", "--net-file", category="input", dest="netfile", type=op.net_file,
                  help="define the net file (mandatory)", required=True)
    op.add_option("-o", "--output-file", category="output", dest="outfile", type=op.additional_file,
                  help="define the output rerouter filename", default="rerouters.xml")
    op.add_option("-x", "--closed-edges", category="input", dest="closedEdges", type=op.edge_list,
                  help="provide a comma-separated list of edges to close")
    op.add_option("-f", "--closed-edges.input-file", category="input", dest="closedEdgesFile", type=op.file,
                  help="provide a selection file with edges to close")
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
    if not options.netfile or (not options.closedEdges and not options.closedEdgesFile):
        op.print_help()
        sys.exit(1)

    options.closedEdges = options.closedEdges.split(',') if options.closedEdges else []
    if options.closedEdgesFile:
        with sumolib.miscutils.openz(options.closedEdgesFile, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line.startswith("edge:"):
                    edgeID = line[5:]
                    options.closedEdges.append(edgeID)

    return options


def findNotifcationEdges(options, net, closedEdges):
    result = set()

    # close edges in the network
    for e in closedEdges:
        for lane in e.getLanes():
            p = set(lane.getPermissions())
            p.remove(options.vclass)
            lane.setPermissions(p)

    reachable = set()
    for e in closedEdges:
        for succ in e.getOutgoing().keys():
            if succ.allows(options.vclass):
                reachable.update(net.getReachable(succ, options.vclass))

    upstream = []
    for e in closedEdges:
        for pred in e.getIncoming().keys():
            if pred.allows(options.vclass):
                upstream.append(pred)

    seen = set()
    cache = {}
    while upstream and reachable:
        cand = upstream.pop(0)
        if cand in seen:
            continue
        seen.add(cand)
        reachable2 = net.getReachable(cand, options.vclass, cache=cache)
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
            print("Error: Unknown closed edge '%s'" % closedID, file=sys.stderr)
            sys.exit(1)
        closedEdges.append(net.getEdge(closedID))

    if not closedEdges:
        print("Error: found no edges to close.", file=sys.stderr)
        sys.exit(1)

    allowDisallow = ""
    if options.disallow is not None:
        allowDisallow = ' disallow="%s"' % options.disallow
    elif options.allow != "":
        allowDisallow = ' allow="%s"' % options.allow

    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)

        rerouterEdges = findNotifcationEdges(options, net, closedEdges)
        if not rerouterEdges:
            print("Warning: No detours found. Rerouter will only close edges.", file=sys.stderr)
            rerouterEdges = closedEdges
        rerouterEdgeIDs = sorted([e.getID() for e in rerouterEdges])

        outf.write('   <rerouter id="%s" edges="%s">\n' % (
                   options.idPrefix,
                   ' '.join(rerouterEdgeIDs)))
        outf.write('      <interval begin="%s" end="%s">\n' % (options.begin, options.end))
        for e in closedEdges:
            outf.write('          <closingReroute id="%s"%s/>\n' % (e.getID(), allowDisallow))

        outf.write('       </interval>\n')
        outf.write('   </rerouter>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    main(get_options())
