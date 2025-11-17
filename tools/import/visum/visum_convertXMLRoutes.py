#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2015-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    visum_convertXMLRoutes.py
# @author  Jakob Erdmann
# @date    Nov 17 2025

"""
Parses a VISUM xml-route file and writes a SUMO route file
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from functools import lru_cache
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import openz  # noqa

def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Import VISUM route file")
    # input
    op.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=op.net_file,
                    help="define the net file (mandatory)")
    op.add_argument("-r", "--route-file", category="input", dest="routefile", required=True, type=op.route_file,
                    help="define the net file (mandatory)")
    # output
    op.add_argument("-o", "--output-trip-file", category="output", dest="outfile", required=True, type=op.route_file,
                    help="define the output route file")
    # processing
    op.add_argument("--vclass", help="Only include routes for the given vclass")
    op.add_argument("-a", "--attributes", default="",
                    help="additional flow attributes.")

    options = op.parse_args(args=args)
    return options


def append_no_duplicate(edges, e):
    if edges and edges[-1] == e:
        return
    edges.append(e)


@lru_cache
def repair(net, prevEdge, edge):
    path, cost = net.getShortestPath(prevEdge, edge)
    if path:
        path = [pe.getID() for pe in path]
        print("Repaired path %s, cost %s" % (path, cost), file=sys.stderr)
        return path
    else:
        print("Found no path from %s to %s" % (prevEdge.getID(), edge.getID()), sys.stderr)
        return None


def main(options):
    vTypes = dict()
    nSkipped = 0
    nDisallowed = 0
    nZeroFlows = 0
    net = sumolib.net.readNet(options.netfile)
    allowed = set()
    if options.vclass:
        for e in net.getEdges():
            if e.allows(options.vclass):
                allowed.add(e.getID())

    edgedict = {}  # (from,to) -> edge
    for e in net.getEdges():
        edgedict[e.getFromNode().getID(), e.getToNode().getID()] = e.getID()

    with openz(options.outfile, 'w') as fout:
        sumolib.writeXMLHeader(fout, "$Id$", "routes", options=options)
        for vtype in sumolib.xml.parse_fast(options.routefile, 'VEHTYPETI', ['INDEX', 'FROMTIME', 'TOTIME', 'VEHTYPEID']):
            vTypes[vtype.INDEX] = (vtype.VEHTYPEID, vtype.FROMTIME, vtype.TOTIME)
            fout.write('    <vType id="%s"/>\n' % vtype.VEHTYPEID)

        nested = {
                'ITEM': ['NODE'],
                'DEMAND': ['VTI', 'VOLUME']}
        for route in sumolib.xml.parse_fast_structured(options.routefile, 'ROUTE', ['INDEX'], nested):
            nodes = [i.NODE for i in route.ITEM]
            if len(nodes) < 2:
                nSkipped += 1
                continue
            edges = []
            prev = None
            for n in nodes:
                if prev is not None:
                    e = edgedict.get((prev, n))
                    if e is not None:
                        append_no_duplicate(edges, e)
                        prev = n
                    elif ' ' in n:
                        _, ex = n.split()
                        e = ex[:-1]
                        if net.hasEdge(e):
                            edge = net.getEdge(e)
                            if edges:
                                prevEdge = net.getEdge(edges[-1])
                                if prevEdge.getToNode() == edge.getFromNode():
                                    append_no_duplicate(edges, e)
                                else:
                                    path = repair(net, prevEdge, edge)
                                    if path:
                                        edges += path[1:]
                            prev = edge.getToNode().getID()
                        else:
                            prev = None
                else:
                    prev = n
                                
            if options.vclass:
                if any([e not in allowed for e in edges]):
                    nDisallowed += 1
                    continue


            fout.write('    <route id="%s" edges="%s"/>\n' % (route.INDEX, ' '.join(edges)))
            for demand in route.DEMAND:
                flowID = "%s_%s" % (route.INDEX, demand.VTI)
                vtype, begin, end = vTypes[demand.VTI]
                rate = float(demand.VOLUME) / 3600
                if rate > 0:
                    attrs = ""
                    if options.attributes:
                        attrs = " " + options.attributes
                    fout.write('    <flow id="%s" route="%s" type="%s" begin="%s" end="%s" period="exp(%s)%s"/>\n' % (
                        flowID, route.INDEX, vtype, begin, end, rate, attrs))
                else:
                    nZeroFlows += 1
        fout.write("</routes>\n")

        if nSkipped > 0:
            print("Warning: Skipped %s routes because they were defined with a single node" % nSkipped)
        if nZeroFlows > 0:
            print("Warning: Skipped %s flows because they have no volume" % nZeroFlows)
        if nDisallowed > 0:
            print("Warning: Ignored %s routes because they have edges that are not allowed for %s " % (
                nDisallowed, options.vclass))

if __name__ == "__main__":
    main(get_options())
