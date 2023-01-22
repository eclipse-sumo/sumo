#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    osmTaxiStop.py
# @author  Michael Behrisch
# @date    2020-10-30

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import io
import random
import argparse
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import osmBuild  # noqa


VEHICLE_LENGTH = 7.5


def parseArgs(args=None):
    argParser = sumolib.options.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    argParser.add_argument("-s", "--osm-file", help="read OSM file from FILE (mandatory)", metavar="FILE")
    argParser.add_argument("-n", "--net-file", help="read SUMO net from FILE", metavar="FILE")
    argParser.add_argument("-o", "--output-file", help="write stopping places to the output FILE", metavar="FILE",
                           default="stopping_places.add.xml")
    argParser.add_argument("-f", "--fleet-file", help="write taxi fleet to the output FILE", metavar="FILE")
    argParser.add_argument("--fleet-size", type=float, metavar="NUM",
                           help="relative (0 < NUM < 1) or absolute number of vehicles (NUM >= 1) to generate")
    argParser.add_argument("-t", "--type", help="stopping place type", default="chargingStation")
    argParser.add_argument("-r", "--radius", type=float, help="radius for edge finding", default=20.)
    argParser.add_argument("-l", "--length", type=float, help="(minimum) length of the stopping place", default=20.)
    argParser.add_argument("--vclass", help="which vehicle class should be allowed", default="passenger")
    return argParser.parse_args(args)


def main(options):
    if options.net_file is None:
        osmBuild.build(["-f", options.osm_file])
        net = sumolib.net.readNet("osm.net.xml")
    else:
        net = sumolib.net.readNet(options.net_file)
    count = 0
    fleet_out = io.open(options.fleet_file, "w", encoding="UTF8") if options.fleet_file else None
    if fleet_out:
        places = []
        sumolib.xml.writeHeader(fleet_out, root="additional")
        print(u"""     <vType id="taxi" vClass="taxi">
        <param key="has.taxi.device" value="true"/>
    </vType>""", file=fleet_out)
    with io.open(options.output_file, "w", encoding="UTF8") as output:
        sumolib.xml.writeHeader(output, root="additional")
        for n in sumolib.xml.parse(options.osm_file, "node"):
            name = None
            bestLane = None
            point = None
            length = options.length
            if n.tag:
                for t in n.tag:
                    if t.k == "capacity":
                        try:
                            length = max(int(t.v) * VEHICLE_LENGTH, length)
                        except ValueError:
                            pass
                    if t.k == "name":
                        name = t.v
                    if t.k == "amenity" and t.v == "taxi":
                        point = net.convertLonLat2XY(float(n.lon), float(n.lat))
                        candidates = net.getNeighboringLanes(*point, r=options.radius, includeJunctions=False)
                        for lane, _ in sorted(candidates, key=lambda i: i[1]):
                            if lane.getLength() > options.length and lane.allows(options.vclass):
                                bestLane = lane
                                break
                if bestLane:
                    pos = sumolib.geomhelper.polygonOffsetWithMinimumDistanceToPoint(point, bestLane.getShape())
                    endPos = min(bestLane.getLength(), max(length, pos + length / 2))
                    nameAttr = 'name="%s" ' % name if name else ""
                    stopID = "%s_%s" % (options.type, count)
                    print(u'    <%s id="%s" %slane="%s" startPos="%.2f" endPos="%.2f"/>' %
                          (options.type, stopID, nameAttr, bestLane.getID(), max(0, endPos - length), endPos),
                          file=output)
                    if fleet_out:
                        places += int(length / VEHICLE_LENGTH) * [stopID]
                    count += 1
        print(u"</additional>", file=output)
    if fleet_out:
        if options.fleet_size:
            fleet_size = int(len(places) * options.fleet_size) if options.fleet_size < 1 else int(options.fleet_size)
            random.seed(42)
            random.shuffle(places)
            places = places[:fleet_size]
        for idx, stopID in enumerate(places):
            print(u'    <trip id="taxi_%s_%s" type="taxi" depart="0.00">' % (stopID, idx), end=u'', file=fleet_out)
            print(u'<stop busStop="%s" triggered="person"/></trip>' % stopID, file=fleet_out)
        print(u"</additional>", file=fleet_out)


if __name__ == "__main__":
    main(parseArgs())
