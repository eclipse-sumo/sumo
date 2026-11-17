#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateDefaultTurnRatios.py
# @author  Mark Colley
# @date    2026-04-12

"""
Generate an edgeRelation turn-ratio XML file from network turning directions
('s', 'l', 'r', 'L', 'R', 't') for use with jtrrouter or routeSampler.py.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import intIfPossible, parseTime  # noqa
from sumolib.net.connection import Connection  # noqa
from sumolib.options import ArgumentParser  # noqa


def get_options(args=None):
    ap = ArgumentParser(description="Generate turn-ratio file based on network turning directions")
    ap.add_argument("-n", "--net-file", dest="netfile", category="input", type=ap.net_file,
                    required=True, help="SUMO network file")
    ap.add_argument("-o", "--output-file", dest="outfile", category="output", type=ap.edgedata_file,
                    required=True, help="output edgeRelation turn-ratio file")
    ap.add_argument("-b", "--begin", default="0", category="time", type=ap.time,
                    help="interval begin time (seconds or H:M:S)")
    ap.add_argument("-e", "--end", default="86400", category="time", type=ap.time,
                    help="interval end time (seconds or H:M:S)")
    ap.add_argument("--id", default="generated", category="output",
                    help="interval id")
    ap.add_argument("-s", "--straight", type=float, default=0.6, category="processing",
                    help="relative weight for straight ('s') turns")
    ap.add_argument("-l", "--left", type=float, default=0.3, category="processing",
                    help="relative weight for left ('l') turns")
    ap.add_argument("-r", "--right", type=float, default=0.1, category="processing",
                    help="relative weight for right ('r') turns")
    ap.add_argument("--partly-left", dest="partlyLeft", type=float, category="processing",
                    help="relative weight for partially-left ('L') turns (defaults to --left)")
    ap.add_argument("--partly-right", dest="partlyRight", type=float, category="processing",
                    help="relative weight for partially-right ('R') turns (defaults to --right)")
    ap.add_argument("-t", "--turnaround", type=float, default=0.0, category="processing",
                    help="relative weight for turnaround ('t') turns")
    ap.add_argument("-j", "--junctions", type=str, category="processing",
                    help="only generate turn ratios at given comma-separated junction IDs")
    ap.add_argument("--tls-only", dest="tlsOnly", action="store_true", default=False,
                    category="processing", help="only generate turn ratios at traffic light junctions")
    ap.add_argument("--vclass", type=str, default="passenger", category="processing",
                    help="only consider connections allowing VCLASS (empty string for all)")
    ap.add_argument("--lanes", action="store_true", default=False, category="processing",
                    help="scale direction weight by number of connecting lanes")
    ap.add_argument("--skip-trivial", dest="skipTrivial", action="store_true", default=False,
                    category="processing", help="skip incoming edges with only a single valid outgoing edge")
    ap.add_argument("--no-normalize", dest="normalize", action="store_false", default=True,
                    category="processing", help="write raw weights instead of normalizing per incoming edge")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)

    options.begin = intIfPossible(parseTime(options.begin))
    options.end = intIfPossible(parseTime(options.end))
    if options.partlyLeft is None:
        options.partlyLeft = options.left
    if options.partlyRight is None:
        options.partlyRight = options.right
    options.junctionSet = set(options.junctions.split(",")) if options.junctions else None
    options.dirWeights = {
        Connection.LINKDIR_STRAIGHT: options.straight,
        Connection.LINKDIR_LEFT: options.left,
        Connection.LINKDIR_RIGHT: options.right,
        Connection.LINKDIR_PARTLEFT: options.partlyLeft,
        Connection.LINKDIR_PARTRIGHT: options.partlyRight,
        Connection.LINKDIR_TURN: options.turnaround,
    }
    return options


def is_tls_junction(node):
    if node.getType().startswith("traffic_light"):
        return True
    for inc in node.getIncoming():
        if inc.getTLS() is not None:
            return True
    return False


def compute_edge_turn_weights(from_edge, options):
    vclass = options.vclass if options.vclass else None
    valid_targets = {}
    for to_edge, conns in from_edge.getOutgoing().items():
        if to_edge.getFunction() == "internal":
            continue
        allowed = [
            c for c in conns
            if vclass is None or (
                c.getFromLane().allows(vclass)
                and c.getToLane().allows(vclass)
                and c.allows(vclass)
            )
        ]
        if not allowed:
            continue
        if options.lanes:
            weight = sum(options.dirWeights.get(c.getDirection(), 0.0) for c in allowed)
        else:
            weight = max(options.dirWeights.get(c.getDirection(), 0.0) for c in allowed)
        valid_targets[to_edge.getID()] = weight

    if not valid_targets:
        return []
    if options.skipTrivial and len(valid_targets) <= 1:
        return []

    non_zero = [(to_id, w) for to_id, w in sorted(valid_targets.items()) if w > 0.0]
    if not non_zero:
        if len(valid_targets) == 1 and options.normalize:
            only_id = next(iter(valid_targets.keys()))
            return [(only_id, 1.0)]
        return []

    if options.normalize:
        total = sum(w for _, w in non_zero)
        return [(to_id, w / total) for to_id, w in non_zero]
    return non_zero


def main(options):
    if options.verbose:
        print("Reading net '%s'" % options.netfile)
    net = sumolib.net.readNet(options.netfile)

    relations_written = 0
    edges_written = 0
    with open(options.outfile, "w", encoding="utf-8") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", schemaPath="datamode_file.xsd", options=options)
        outf.write('    <interval id="%s" begin="%s" end="%s">\n' % (
            options.id, options.begin, options.end))
        for from_edge in sorted(net.getEdges(), key=lambda e: e.getID()):
            if from_edge.getFunction() == "internal":
                continue
            to_node = from_edge.getToNode()
            if to_node is None:
                continue
            if options.junctionSet is not None and to_node.getID() not in options.junctionSet:
                continue
            if options.tlsOnly and not is_tls_junction(to_node):
                continue
            pairs = compute_edge_turn_weights(from_edge, options)
            if not pairs:
                continue
            edges_written += 1
            for to_id, prob in pairs:
                outf.write('        <edgeRelation from="%s" to="%s" probability="%.4f"/>\n' % (
                    from_edge.getID(), to_id, prob))
                relations_written += 1
        outf.write('    </interval>\n')
        outf.write('</data>\n')

    if options.verbose:
        print("Wrote %d turn ratios for %d incoming edges to '%s'" % (
            relations_written, edges_written, options.outfile))


if __name__ == "__main__":
    main(get_options())
