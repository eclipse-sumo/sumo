#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    addTurnLanes.py
# @author  Jakob Erdmann
# @date    2026-11-19

"""
Generate an edge-patch file with <split> elements adding turn lanes near the downstream
junction of selected (or multi-movement) edges.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(THIS_DIR)
sys.path.append(os.path.join(THIS_DIR, ".."))
import sumolib  # noqa
from sumolib.geomhelper import polyLength  # noqa
from reduceLanes import read_selected_edges  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Generate <split> edge patches adding turn lanes near junctions")
    ap.add_argument("-n", "--network", category="input", type=ap.net_file, required=True,
                    help="SUMO network to analyze")
    ap.add_argument("-s", "--selection-file", dest="selectionFile", category="input", type=ap.file,
                    help="Selection file listing edges to equip with turn lanes (all multi-turn edges if omitted)")
    ap.add_argument("-o", "--output-file", dest="outputFile", category="output", type=ap.edgedata_file,
                    default="turnlanes.edg.xml", help="Output edge patch XML file")
    ap.add_argument("-l", "--turn-lane-length", dest="turnLength", type=float, default=20.0,
                    help="Length in meters of the added turn-lane section upstream of the junction (default 20)")
    ap.add_argument("--add-lanes", dest="addLanes", type=int, default=1,
                    help="Number of turn lanes to add (default 1)")
    ap.add_argument("--side", choices=("left", "right"), default="left",
                    help="Side on which turn lanes are added ('left' or 'right', default 'left')")
    ap.add_argument("--min-length", dest="minLength", type=float, default=40.0,
                    help="Minimum edge length required to split (default 40)")
    ap.add_argument("--suffix", default=".turn",
                    help="Suffix appended to the downstream widened edge ID (default '.turn')")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Print summary of modified and skipped edges")
    options = ap.parse_args(args=args)
    if options.addLanes < 1:
        sys.exit("--add-lanes must be >= 1.")
    if options.turnLength <= 0:
        sys.exit("--turn-lane-length must be > 0.")
    return options


def format_num(val):
    return "%g" % val if float(val).is_integer() else ("%.2f" % val).rstrip("0").rstrip(".")


def main(args=None):
    options = get_options(args=args)
    net = sumolib.net.readNet(options.network)
    selected = read_selected_edges(options.selectionFile) if options.selectionFile else None

    all_roundabouts = set()
    for r in net.getRoundabouts():
        all_roundabouts.update(r.getEdges())

    if selected is not None:
        unknown = sorted(eid for eid in selected if not net.hasEdge(eid))
        for eid in unknown:
            print("Warning: Unknown selected edge '%s'." % eid, file=sys.stderr)

    modified = 0
    too_short = 0
    skipped_roundabout = 0

    with sumolib.openz(options.outputFile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for edge in net.getEdges():
            eid = edge.getID()
            if selected is not None:
                if eid not in selected:
                    continue
            else:
                if len(edge.getOutgoing()) <= 1:
                    continue

            if eid in all_roundabouts:
                skipped_roundabout += 1
                continue

            length = min(polyLength(edge.getShape()), edge.getLength())
            req_len = max(options.minLength, options.turnLength + 1.0)
            if length < req_len:
                too_short += 1
                if selected is not None:
                    print("Warning: Edge '%s' (%.2fm) is shorter than minimum length %.2fm; skipping." % (
                        eid, length, req_len), file=sys.stderr)
                continue

            orig_lanes = edge.getLaneNumber()
            new_lanes = orig_lanes + options.addLanes
            if options.side == "left":
                upstream_lanes = " ".join(str(i) for i in range(orig_lanes))
            else:
                upstream_lanes = " ".join(str(i) for i in range(options.addLanes, new_lanes))
            downstream_lanes = " ".join(str(i) for i in range(new_lanes))

            outf.write('    <edge id=%s>\n' % sumolib.xml.quoteattr(eid))
            outf.write('        <split pos="0" lanes="%s"/>\n' % upstream_lanes)
            outf.write('        <split pos="-%s" lanes="%s" idBefore=%s idAfter=%s/>\n' % (
                format_num(options.turnLength),
                downstream_lanes,
                sumolib.xml.quoteattr(eid),
                sumolib.xml.quoteattr(eid + options.suffix),
            ))
            outf.write('    </edge>\n')
            modified += 1
        outf.write("</edges>\n")

    print("Added turn-lane splits for %d edges (%d too short, %d roundabouts)." % (
        modified, too_short, skipped_roundabout))


if __name__ == "__main__":
    main()
