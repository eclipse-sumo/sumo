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

# @file    driveways2poly.py
# @author  Jakob Erdmann
# @date    2024-06-14

"""
Visualize railsignal-block-output as polygons
which can be loaded with sumo-gui for visualization
"""
from __future__ import absolute_import
import sys
import os
import random
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import sumolib  # noqa
import route2poly  # noqa


def parse_args(args=None):
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-n", "--netfile", required=True, help="name of input network file")
    optParser.add_option("-d", "--driveways", required=True, help="name of input railsignal block file")
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
    optParser.add_option("--filter-signals", dest="filterSignals",
                         help="only write output for driveways of the given signals")
    optParser.add_option("--filter-driveways", dest="filterDriveways",
                         help="only write output for the given driveways")
    optParser.add_option("--filter-foes", dest="filterFoes",
                         help="only write output for the foes of the given driveways")
    optParser.add_option("--seed", type=int, help="random seed")

    options = optParser.parse_args(args=args)
    if options.seed:
        random.seed(options.seed)

    if options.filterSignals:
        options.filterSignals = set(options.filterSignals.split(','))
    if options.filterDriveways:
        options.filterDriveways = set(options.filterDriveways.split(','))
    if options.filterFoes:
        options.filterFoes = set(options.filterFoes.split(','))

    return options


def getDriveWays(fname):
    for rs in sumolib.xml.parse(fname, "railSignal"):
        for link in rs.link:
            if not link.driveWay:
                continue
            for dw in link.driveWay:
                yield rs.id, dw
    for dj in sumolib.xml.parse(fname, "departJunction"):
        for dw in dj.driveWay:
            yield dj.id, dw


def main(options):
    colorgen = sumolib.miscutils. Colorgen((options.hue, options.saturation, options.brightness))
    net = sumolib.net.readNet(options.netfile, withInternal=options.internal)

    permittedFoes = None
    if options.filterFoes:
        permittedFoes = set()
        for signal, dw in getDriveWays(options.driveways):
            if dw.id in options.filterFoes:
                permittedFoes.update(dw.foes[0].driveWays.split())

    with open(options.output, 'w') as outf:
        sumolib.xml.writeHeader(outf, root='polygons', rootAttrs=None, options=options)
        for signal, dw in getDriveWays(options.driveways):
            if options.filterSignals and signal not in options.filterSignals:
                continue
            if options.filterDriveways and dw.id not in options.filterDriveways:
                continue
            if permittedFoes and dw.id not in permittedFoes:
                continue
            route2poly.generate_poly(options, net, dw.id, colorgen(), dw.edges.split(), outf)
        outf.write('</polygons>\n')


if __name__ == "__main__":
    main(parse_args())
