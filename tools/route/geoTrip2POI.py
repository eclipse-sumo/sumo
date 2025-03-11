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

# @file    geoTrip2POI.py
# @author  Jakob Erdmann
# @date    2025-01-16

"""
Load a file with trips define with fromLonLat / toLonLat and convert it to a poi file
with POIs scaled to the relative amount of departs / arrivals
"""
from __future__ import absolute_import
import sys
import os
import random
import colorsys
from collections import defaultdict
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse  # noqa


def parse_args(args):
    op = sumolib.options.ArgumentParser(description="convert geoTrips to POIs")
    op.add_argument("routeFiles", nargs="+", category="input", type=op.file,
                    help="trip files to analyze")
    op.add_argument("-o", "--output-file", category="output", dest="outfile",
                    help="output poi file")
    op.add_argument("-l", "--layer", default=100, help="layer for generated polygons")
    op.add_argument("--scale-width", type=float, dest="scaleWidth", default=10,
                    help="Scale with of POIs by factor")
    op.add_argument("--filter-output.file", dest="filterOutputFile",
                    help="only write output for edges in the given selection file")
    op.add_argument("--seed", type=int, help="random seed")
    options = op.parse_args()
    if options.seed:
        random.seed(options.seed)
    if options.outfile is None:
        options.outfile = options.routefiles[0] + ".poi.xml"

    return options


def main(args):
    options = parse_args(args)
    departLocs = defaultdict(lambda: 0)
    arrivalLocs = defaultdict(lambda: 0)

    for routefile in options.routeFiles:
        for trip in parse(routefile, 'trip'):
            departLocs[trip.fromLonLat] += 1
            arrivalLocs[trip.toLonLat] += 1

    maxCount = max(
        max(departLocs.values()),
        max(arrivalLocs.values()))

    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, root="additional", options=options)
        for comment, counts, rgb in (
                ('departures', departLocs, (1, 0, 0)),
                ('arrivals', arrivalLocs, (0, 0, 1))):

            outf.write('<!-- %s -->\n' % comment)
            hue, sat, val = colorsys.rgb_to_hsv(*rgb)
            for i, (lonLat, count) in enumerate(counts.items()):
                frac = 0.2 + (count / maxCount) * 0.8
                lon, lat = lonLat.split(',')
                outf.write('  <poi id="%s%s" layer="%s" color="%s,1" width="%s" type="%s" lon="%s" lat="%s"/>\n' % (
                    comment[0], i,
                    options.layer,
                    ','.join(map(str, colorsys.hsv_to_rgb(hue, frac, val))),
                    options.scaleWidth * frac + 1,
                    count,
                    lon, lat))
        outf.write('</additional>\n')


if __name__ == "__main__":
    main(sys.argv[1:])
