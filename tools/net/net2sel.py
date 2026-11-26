#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    net2sel.py
# @author  Jakob Erdmann
# @date    2025-07-16

"""
This script converts the network to a selection file of junctions and edges
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--net-file", dest="netFile", required=True,
                           category="input", type=argParser.net_file, help="The .net.xml file to convert")
    argParser.add_argument("-o", "--output-file", dest="outFile", required=True, category="output",
                           type=argParser.file, help="The polygon output file name")
    argParser.add_argument("-l", "--lanes", action="store_true", default=False,
                           category="processing", help="Write lanes to selection instead of edges")
    argParser.add_argument("--strongly-connected", dest="strongCount", type=int, default=0,
                           category="processing", help="Keep only the INT largest strongly connected components")
    argParser.add_argument("--vclasses", default=None, category="processing",
                           help="Comma-separated vehicle classes requiring strong connectivity")
    return argParser.parse_args()


def getStronglyConnectedEdges(net, count, vclasses):
    vcs = [v.strip() for v in vclasses.split(",") if v.strip()] if vclasses else []
    edges = [e for e in net.getEdges() if all(e.allows(vc) for vc in vcs)]
    allowed = {e.getID() for e in edges}
    adj = {e.getID(): [o.getID() for o, cons in e.getOutgoing().items()
                       if o.getID() in allowed and all(any(c.allows(vc) for c in cons) for vc in vcs)]
           for e in edges}
    idx, low, on_stack, stack, sccs = {}, {}, set(), [], []
    counter = 0
    for start in sorted(adj):
        if start in idx:
            continue
        call_stack = [(start, 0)]
        idx[start] = low[start] = counter
        counter += 1
        stack.append(start)
        on_stack.add(start)
        while call_stack:
            u, pos = call_stack[-1]
            nbrs = adj[u]
            if pos < len(nbrs):
                call_stack[-1] = (u, pos + 1)
                w = nbrs[pos]
                if w not in idx:
                    idx[w] = low[w] = counter
                    counter += 1
                    stack.append(w)
                    on_stack.add(w)
                    call_stack.append((w, 0))
                elif w in on_stack:
                    low[u] = min(low[u], idx[w])
            else:
                call_stack.pop()
                if call_stack:
                    p = call_stack[-1][0]
                    low[p] = min(low[p], low[u])
                if low[u] == idx[u]:
                    comp = []
                    while True:
                        w = stack.pop()
                        on_stack.remove(w)
                        comp.append(w)
                        if w == u:
                            break
                    sccs.append(sorted(comp))
    sccs.sort(key=lambda c: (-len(c), c[0]))
    kept = {eid for comp in sccs[:count] for eid in comp}
    return [e for e in net.getEdges() if e.getID() in kept]


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.netFile)
    edges = (getStronglyConnectedEdges(net, options.strongCount, options.vclasses)
             if options.strongCount > 0 else net.getEdges())
    nodes = ({e.getFromNode() for e in edges} | {e.getToNode() for e in edges}
             if options.strongCount > 0 else net.getNodes())

    with open(options.outFile, 'w') as outf:
        for junction in sorted(nodes, key=lambda n: n.getID()) if options.strongCount > 0 else nodes:
            outf.write("junction:%s\n" % junction.getID())
        for edge in edges:
            if options.lanes:
                for lane in edge.getLanes():
                    outf.write("lane:%s\n" % lane.getID())
            else:
                outf.write("edge:%s\n" % edge.getID())
