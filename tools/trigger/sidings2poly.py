#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    sidings2poly.py
# @author  Jakob Erdmann
# @date    2025-10-14

"""
Visualize overtakingReroutes as polygons
which can be loaded with sumo-gui for visualization
"""
from __future__ import absolute_import
import sys
import os
import random
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'route'))
import sumolib  # noqa
import route2poly  # noqa


def parse_args(args=None):
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-n", "--netfile", required=True, help="name of input network file")
    optParser.add_option("-r", "--rerouter-file", required=True, dest="rrFile",
                         help="name of input railsignal block file")
    optParser.add_option("-o", "--output", required=True, help="name of output file")
    optParser.add_option("-u", "--hue", default="random",
                         help="hue for polygons (float from [0,1] or 'random')")
    optParser.add_option("-s", "--saturation", default=1,
                         help="saturation for polygons (float from [0,1] or 'random')")
    optParser.add_option("-b", "--brightness", default=1,
                         help="brightness for polygons (float from [0,1] or 'random')")
    optParser.add_option("-l", "--layer", default=100, help="layer for generated polygons")
    optParser.add_option("--geo", action="store_true",
                         default=False, help="write polygons with geo-coordinates")
    optParser.add_option("--internal", action="store_true",
                         default=False, help="include internal edges in generated shapes")
    optParser.add_option("--spread", type=float, help="spread polygons laterally to avoid overlap")
    optParser.add_option("--blur", type=float,
                         default=0, help="maximum random disturbance to route geometry")
    optParser.add_option("--scale-width", type=float, dest="scaleWidth",
                         help="group similar routes and scale width by " +
                              "group size multiplied with the given factor (in m)")
    optParser.add_option("--filter-rerouters", dest="filterRR",
                         help="only write output for the given list of rerouters")
    optParser.add_option("--seed", type=int, help="random seed")

    options = optParser.parse_args(args=args)
    if options.seed:
        random.seed(options.seed)

    if options.filterRR:
        options.filterRR = set(options.filterRR.split(','))

    return options


def getOvertakingReroutes(fname):
    for rerouter in sumolib.xml.parse(fname, "rerouter"):
        for interval in rerouter.interval:
            if interval.overtakingReroute:
                for otr in interval.overtakingReroute:
                    yield rerouter.id, otr.main, otr.siding


def darker(col, factor):
    return tuple([min(255, max(0, int(factor * b))) for b in col])


def main(options):
    colorgen = sumolib.miscutils.Colorgen((options.hue, options.saturation, options.brightness))
    net = sumolib.net.readNet(options.netfile, withInternal=options.internal)

    with open(options.output, 'w') as outf:
        sumolib.xml.writeHeader(outf, root='polygons', rootAttrs=None, options=options)
        rrPrev = None
        color = None
        color2 = None
        index = 0
        for rrid, main, siding in getOvertakingReroutes(options.rrFile):
            if options.filterRR and rrid not in options.filterRR:
                continue
            if rrPrev != rrid:
                index = 0
                color = colorgen.byteTuple()
                color2 = darker(color, 0.7)
                color = ",".join(map(str, color))
                color2 = ",".join(map(str, color2))
                rrPrev = rrid
            p_id = "%s_%s" % (rrid, index)
            index += 1
            route2poly.generate_poly(options, net, p_id + "_main",  color, main.split(), outf)
            route2poly.generate_poly(options, net, p_id + "_siding", color2, siding.split(), outf)
        outf.write('</polygons>\n')


if __name__ == "__main__":
    main(parse_args())
