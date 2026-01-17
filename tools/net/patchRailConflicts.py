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

# @file    patchRailCrossings.py
# @author  Jakob Erdmann
# @date    2026-01-17

"""
Identifies all rail crossings (two one-directional tracks crossing each other)
and converts the junction type to the given value
"""
import os
import sys
import subprocess
from collections import defaultdict
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa

try:
    sys.stdout.reconfigure(encoding='utf-8')
except:  # noqa
    pass

NETCONVERT = sumolib.checkBinary('netconvert')

def get_options():
    ap = ArgumentParser()
    ap.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=ap.net_file,
                    help="the network to read lane and edge permissions")
    ap.add_argument("-o", "--output-file", category="output", dest="output", required=True, type=ap.file,
                    help="output network file")
    ap.add_argument("--vclass", default="tram",
                    help="the vehicle class that restricts which junctions are considered")
    ap.add_argument("-t", "--junction-type", dest="junctionType", default="rail_signal",
                    help="the new junction type for rail/rail crossings")
    ap.add_argument("-k", "--keep-junction-type", dest="keepJunctionType", default="traffic_light,traffic_light_unregulated,rail_signal",
                    help="the new junction type for rail/rail crossings")
    ap.add_argument("-e", "--end-offset", dest="endOffset", type=float, default=0,
                    help="move back the stop line from the crossing")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args()

    outputBase = options.output
    if outputBase[-8:] == ".net.xml":
        outputBase = outputBase[:-8]
    elif outputBase[-11:] == ".net.xml.gz":
        outputBase = outputBase[:-11]

    options.vclass = [options.vclass]
    options.keepJunctionType = set(options.keepJunctionType.split(','))
    options.output_nodes = outputBase + ".nod.xml"
    options.output_edges = outputBase + ".edg.xml"
    return options


def main(options):
    net = sumolib.net.readNet(options.netfile)
    crossingNodes = []
    skipped = defaultdict(lambda: 0)

    for node in net.getNodes():
        if any([e.getPermissions() != options.vclass for e in node.getIncoming() + node.getOutgoing()]):
            continue
        nIn = len(node.getIncoming())
        nOut = len(node.getOutgoing())
        nBidi = len([e for e in node.getIncoming() + node.getOutgoing() if e.getBidi() is not None]) / 2
        if nIn >= 2 and nOut >= 1 and ((nIn + nOut - nBidi) >= 3):
            if node.getType() in options.keepJunctionType:
                skipped[node.getType()] += 1
                continue
            crossingNodes.append(node)

    with open(options.output_nodes, 'w') as outf_nod:
        sumolib.writeXMLHeader(outf_nod, "$Id$", "nodes", options=options)
        for nodeID in sorted([n.getID() for n in crossingNodes]):
            outf_nod.write('    <node id="%s" type="%s"/>\n' % (nodeID, options.junctionType))
        outf_nod.write("</nodes>\n")

    with open(options.output_edges, 'w') as outf_edg:
        sumolib.writeXMLHeader(outf_edg, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        incoming = []
        for node in crossingNodes:
            incoming += [e.getID() for e in node.getIncoming()]
        incoming.sort()
        for e in incoming:
            outf_edg.write('    <edge id="%s" endOffset="%s"/>\n' % (e, options.endOffset))
        outf_edg.write("</edges>\n")

    if options.verbose:
        if skipped:
            for t, n in skipped.items():
                print("Skipped %s crossings of type %s" % (n, t))

        print("Building new net")
    sys.stderr.flush()
    subprocess.call([NETCONVERT,
                     '-s', options.netfile,
                     '-n', options.output_nodes,
                     '-e', options.output_edges,
                     '-o', options.output,
                     ], stdout=subprocess.DEVNULL)

if __name__ == "__main__":
    main(get_options())
