#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    meandata2vss.py
# @author  Michael Behrisch
# @date    2023-09-08

from __future__ import absolute_import

import os
import sys
import collections
import warnings
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


def parse_args():
    op = sumolib.options.ArgumentParser()
    op.add_argument("-o", "--outfile", help="name of output file")
    op.add_argument("-n", "--net-file", category="processing", type=op.net_file,
                    help="parse net for edge lengths")
    op.add_argument("-s", "--scale", type=float, default=1., help="scaling factor for speed")
    op.add_argument("-d", "--minimum-deviation", type=float, default=0.,
                    help="minimum relative deviation before applying a vss (needs a network)")
    op.add_argument("-a", "--attribute", help="attribute to parse for speed")
    op.add_argument("meandatafile", help="name of input file")
    options = op.parse_args()
    if options.outfile is None:
        options.outfile = options.meandatafile + ".add.xml"
    return options


def main():
    options = parse_args()
    edge_intervals = collections.defaultdict(list)
    net = sumolib.net.readNet(options.net_file) if options.net_file else None
    for interval in sumolib.xml.parse(options.meandatafile, "interval"):
        for edge in interval.edge:
            if options.attribute:
                if not getattr(edge, options.attribute):
                    warnings.warn("Skipping edge which has no attribute '%s'." % options.attribute)
                    continue
                speed = float(getattr(edge, options.attribute))
            elif edge.speed is None:
                if edge.traveltime is None:
                    warnings.warn("Skipping edge which has neither 'speed' nor 'traveltime'.")
                    continue
                if net is None:
                    warnings.warn("No network, cannot calculate speed from travel time.")
                    continue
                speed = net.getEdge(edge.id).getLength() / float(edge.traveltime)
            else:
                speed = float(edge.speed)
            if net and options.minimum_deviation:
                edge_speed = net.getEdge(edge.id).getSpeed()
                if abs((speed - edge_speed) / edge_speed) < options.minimum_deviation:
                    continue
            speed *= options.scale
            edge_intervals[edge.id].append((interval.begin, interval.end, speed))
    with open(options.outfile, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="additional", options=options)
        for edge, times in edge_intervals.items():
            num_lanes = net.getEdge(edge).getLaneNumber() if net else 1
            lanes = " ".join(["%s_%s" % (edge, idx) for idx in range(num_lanes)])
            outf.write('    <variableSpeedSign id="vss_%s" lanes="%s">\n' % (edge, lanes))
            prev = None
            for entry in times:
                if prev and entry[0] != prev[1]:
                    outf.write('        <step time="%s"/>\n' % prev[1])
                outf.write('        <step time="%s" speed="%.2f"/>\n' % entry[0::2])
                prev = entry
            outf.write('    </variableSpeedSign>\n')
        outf.write("</additional>\n")


if __name__ == "__main__":
    main()
