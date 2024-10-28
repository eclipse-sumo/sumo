#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    mapDetectors.py
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2022-04-25

"""
Create detector definitions by map-matching coordinates to a .net.xml file
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import csv
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(
        description="Map detector locations to a network and write inductionLoop-definitions")
    optParser.add_argument("-n", "--net-file", dest="netfile", category="input", type=optParser.net_file,
                           help="define the net file (mandatory)")
    optParser.add_argument("-d", "--detector-file", dest="detfile", category="input", type=optParser.file,
                           help="csv input file with detector ids and coordinates")
    optParser.add_argument("--delimiter", default=";",
                           help="the field separator of the detector input file")
    optParser.add_argument("-o", "--output-file", dest="outfile", category="output", type=optParser.file,
                           help="define the output file for generated mapped detectors")
    optParser.add_argument("-i", "--id-column", default="id", dest="id",
                           help="Read detector ids from the given column")
    optParser.add_argument("-x", "--longitude-column", default="lon", dest="lon",
                           help="Read detector x-coordinate (lon) from the given column")
    optParser.add_argument("-y", "--latitude-column", default="lat", dest="lat",
                           help="Read detector y-coordinate (lat) from the given column")
    optParser.add_argument("--max-radius", type=float, default="1000", dest="maxRadius",
                           help="specify maximum distance error when mapping coordinates")
    optParser.add_argument("--vclass", default="passenger",
                           help="only consider edges that permit the given vClass")
    optParser.add_argument("--det-output-file", dest="detOut", default="detector.out.xml", category="output",
                           type=optParser.file, help="Define the output file that generated detectors shall write to")
    optParser.add_argument("--interval", default=3600, type=optParser.time,
                           help="Define the aggregation interval of generated detectors")
    optParser.add_argument("-v", "--verbose", action="store_true",
                           default=False, help="tell me what you are doing")
    options = optParser.parse_args(args=args)
    if not options.netfile or not options.detfile or not options.outfile:
        optParser.print_help()
        sys.exit(1)

    return options


def main():
    options = get_options()
    net = sumolib.net.readNet(options.netfile)
    with sumolib.openz(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, root="additional", options=options)
        inputf = sumolib.openz(options.detfile)
        reader = csv.DictReader(inputf, delimiter=options.delimiter)
        checkedFields = False
        for row in reader:
            if not checkedFields:
                checkedFields = True
                for attr in ["id", "lon", "lat"]:
                    colName = getattr(options, attr)
                    if colName not in row:
                        sys.exit("Required column %s not found. Available columns are %s" % (
                                 colName, ",".join(row.keys())))
            detID = row[options.id]
            lon = float(row[options.lon])
            lat = float(row[options.lat])
            x, y = net.convertLonLat2XY(lon, lat)

            lanes = []
            radius = 0.1
            factor = 10
            while not lanes and radius <= options.maxRadius:
                lanes = net.getNeighboringLanes(x, y, radius, True)
                lanes = [(d, lane) for lane, d in lanes if lane.allows(options.vclass)]
                radius *= factor
            if not lanes:
                sys.stderr.write("Could not find road for detector %s within %sm radius\n" % (detID, options.maxRadius))
                continue
            lanes.sort(key=lambda x: x[0])
            best = lanes[0][1]
            pos = min(best.getLength(),
                      sumolib.geomhelper.polygonOffsetWithMinimumDistanceToPoint((x, y), best.getShape()))
            outf.write('    <inductionLoop id="%s" lane="%s" pos="%.2f" file="%s" freq="%s"/>\n' % (
                       detID, best.getID(), pos, options.detOut, options.interval))

        outf.write('</additional>\n')
        inputf.close()


if __name__ == "__main__":
    main()
