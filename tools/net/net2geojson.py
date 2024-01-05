#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    net2geojson.py
# @author  Jakob Erdmann
# @date    2020-05-05

"""
This script converts a sumo network to GeoJSON and optionally includes edgeData
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import json
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def parse_args():
    op = sumolib.options.ArgumentParser(description="net to geojson",
                                        usage="Usage: " + sys.argv[0] + " -n <net> <options>")
    # input
    op.add_argument("-n", "--net-file", category="input", dest="netFile", required=True, type=op.net_file,
                    help="The .net.xml file to convert")
    op.add_argument("-d", "--edgedata-file", category="input", dest="edgeData", type=op.edgedata_file,
                    help="Optional edgeData to include in the output")
    op.add_argument("-p", "--ptline-file", category="input", dest="ptlines", type=op.file,
                    help="Optional ptline information to include in the output")
    # output
    op.add_argument("-o", "--output-file", dest="outFile", category="output", required=True, type=op.file,
                    help="The geojson output file name")
    # processing
    op.add_argument("-l", "--lanes", action="store_true", default=False,
                    help="Export lane geometries instead of edge geometries")
    op.add_argument("--junctions", action="store_true", default=False,
                    help="Export junction geometries")
    op.add_argument("-i", "--internal", action="store_true", default=False,
                    help="Export internal geometries")
    op.add_argument("-j", "--junction-coordinates", dest="junctionCoords", action="store_true", default=False,
                    help="Append junction coordinates to edge shapes")
    op.add_argument("-b", "--boundary", dest="boundary", action="store_true", default=False,
                    help="Export boundary shapes instead of center-lines")
    op.add_argument("--edgedata-timeline", action="store_true", default=False, dest="edgedataTimeline",
                    help="exports all time intervals (by default only the first is exported)")

    try:
        options = op.parse_args()
    except (NotImplementedError, ValueError) as e:
        print(e, file=sys.stderr)
        sys.exit(1)
    return options


def shape2json(net, geometry, isBoundary):
    lonLatGeometry = [net.convertXY2LonLat(x, y) for x, y in geometry]
    coords = [[round(x, 6), round(y, 6)] for x, y in lonLatGeometry]
    if isBoundary:
        coords = [coords]
    return {
        "type": "Polygon" if isBoundary else "LineString",
        "coordinates": coords
    }


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.netFile, withInternal=options.internal)
    if not net.hasGeoProj():
        sys.stderr.write("Network does not provide geo projection\n")
        sys.exit(1)

    edgeData = defaultdict(dict)
    if options.edgeData:
        for i, interval in enumerate(sumolib.xml.parse(options.edgeData, "interval", heterogeneous=True)):
            for edge in interval.edge:
                data = dict(edge.getAttributes())
                data["begin"] = interval.begin
                data["end"] = interval.end
                del data["id"]
                edgeData[edge.id][i] = data
            if not options.edgedataTimeline:
                break

    ptLines = defaultdict(lambda: defaultdict(set))
    if options.ptlines:
        for ptline in sumolib.xml.parse(options.ptlines, "ptLine", heterogeneous=True):
            if ptline.route:
                for edge in ptline.route[0].edges.split():
                    ptLines[edge][ptline.type].add(ptline.line)

    features = []

    geomType = 'lane' if options.lanes else 'edge'
    for id, geometry, width in net.getGeometries(options.lanes, options.junctionCoords):
        feature = {"type": "Feature"}
        feature["properties"] = {
            "element": geomType,
            "id": id,
        }
        edgeID = net.getLane(id).getEdge().getID() if options.lanes else id
        if edgeID in edgeData:
            if options.edgedataTimeline:
                feature["properties"]["edgeData"] = edgeData[edgeID]
            else:
                feature["properties"].update(edgeData[edgeID][0])

        if edgeID in ptLines:
            for ptType, lines in ptLines[edgeID].items():
                feature["properties"][ptType] = " ".join(sorted(lines))

        feature["properties"]["name"] = net.getEdge(edgeID).getName()
        if options.boundary:
            geometry = sumolib.geomhelper.line2boundary(geometry, width)
        feature["geometry"] = shape2json(net, geometry, options.boundary)
        features.append(feature)

    if options.junctions:
        for junction in net.getNodes():
            feature = {"type": "Feature"}
            feature["properties"] = {
                "element": 'junction',
                "id": junction.getID(),
            }
            feature["geometry"] = shape2json(net, junction.getShape(), options.boundary)
            features.append(feature)

    geojson = {}
    geojson["type"] = "FeatureCollection"
    geojson["features"] = features
    with open(options.outFile, 'w') as outf:
        outf.write(json.dumps(geojson, sort_keys=True, indent=4, separators=(',', ': ')))
