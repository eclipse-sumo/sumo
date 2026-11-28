#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    reduceLanes.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2016-12-08

"""
build a patch file for the given network that reduces the lanes
for edges with more than 2 lanes within a specified range of its nodes
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import sumolib  # noqa
from sumolib.geomhelper import polyLength  # noqa


def read_selected_edges(selection_file):
    selected = set()
    with sumolib.openz(selection_file) as sf:
        for raw in sf:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            if line.startswith("edge:"):
                selected.add(line[5:].strip())
            elif ":" not in line:
                selected.add(line)
    return selected


def parse_args(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("-n", "--network", category="input", type=ap.net_file, required=True, help="sumo network to use")
    ap.add_argument("-s", "--selection-file", dest="selection_file", category="input", type=ap.file,
                    help="Optional selection file restricting which edges are modified")
    ap.add_argument("-o", "--output-file", category="output", type=ap.edgedata_file,
                    default="reduced.edg.xml", help="The output edge patch file name")
    ap.add_argument("--too-short-output", category="output", type=ap.edgedata_file,
                    help="The output for edges which were ignored because of length")
    ap.add_argument("--roundabout-output", category="output", type=ap.edgedata_file,
                    help="The output for edges which were ignored because of roundabouts")
    ap.add_argument("--min-length", type=float, default=60.,
                    help="the minimum edge length to process")
    ap.add_argument("--min-lane-number", type=int, default=1,
                    help="the minimum number of lanes to process")
    ap.add_argument("--junction-distance", type=float, default=20.,
                    help="where to perform the edge split near the junction")
    ap.add_argument("--max-priority", type=int, default=13,
                    help="the maximum priority")
    return ap.parse_args(args=args)


def main(options):
    sys.stdout.write("Loading net %s ..." % options.network)
    sys.stdout.flush()
    net = sumolib.net.readNet(options.network)
    sys.stdout.write(" done.\n")

    selected = read_selected_edges(options.selection_file) if options.selection_file else None

    allRoundabouts = set()
    for r in net.getRoundabouts():
        allRoundabouts.update(r.getEdges())

    modifiedEdges = 0
    tooShort = set()
    roundabouts = set()
    with open(options.output_file, 'w') as f:
        f.write('<edges>\n')
        totalLength = 0.
        totalMultiLaneLength = 0.
        totalReduced = 0.
        for edge in net.getEdges():
            edgeID = edge.getID()
            length = min(polyLength(edge.getShape()), edge.getLength())
            totalLength += length
            if edge.getLaneNumber() > options.min_lane_number:
                totalMultiLaneLength += length
            if selected is not None and edgeID not in selected:
                continue
            if edge.getPriority() <= options.max_priority and edge.getLaneNumber() > options.min_lane_number:
                if any([lane.getPermissions() in (set(["bus"]), set(["tram"])) for lane in edge.getLanes()]):
                    continue
                if length >= options.min_length:
                    if edgeID not in allRoundabouts:
                        modifiedEdges += 1
                        lanes = edge.getLaneNumber()
                        f.write('    <edge id="%s">\n' % edgeID)
                        f.write('        <split lanes="%s" pos="%s" idBefore="%s"/>\n' %
                                (" ".join(map(str, range(lanes)[1:])), options.junction_distance, edgeID + ".before"))
                        f.write('        <split pos="%s" idBefore="%s" idAfter="%s"/>\n' %
                                (-options.junction_distance, edgeID, edgeID + ".after"))
                        f.write('    </edge>\n')
                        totalReduced += length - 2 * options.junction_distance
                    else:
                        roundabouts.add(edgeID)
                else:
                    tooShort.add(edgeID)
        f.write('</edges>\n')
    print("added splits for %s edges (%s were to short to qualify and %s were roundabouts)" %
          (modifiedEdges, len(tooShort), len(roundabouts)))
    print("total road length: %.2fm, %.2fm total lane reduced length: %.2fm." %
          (totalLength, totalMultiLaneLength, totalReduced))

    if tooShort and options.too_short_output:
        with open(options.too_short_output, 'w') as f:
            for edgeID in sorted(tooShort):
                f.write("edge:%s\n" % edgeID)

    if roundabouts and options.roundabout_output:
        with open(options.roundabout_output, 'w') as f:
            for edgeID in sorted(roundabouts):
                f.write("edge:%s\n" % edgeID)


if __name__ == "__main__":
    main(parse_args())
