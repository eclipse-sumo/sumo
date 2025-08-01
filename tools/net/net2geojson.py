#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2025 German Aerospace Center (DLR) and others.
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
from __future__ import absolute_import, print_function

import json
import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
import sumolib.geomhelper as gh


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
                    help="Export lane geometries")
    op.add_argument("-e", "--edges", action="store_true", default=False, help="Export edge geometries")
    op.add_argument("--junctions", action="store_true", default=False,
                    help="Export junction geometries")
    op.add_argument("-i", "--internal", action="store_true", default=False,
                    help="Export internal geometries")
    op.add_argument("-j", "--junction-coordinates", dest="junctionCoords", action="store_true", default=False,
                    help="Append junction coordinates to edge shapes")
    op.add_argument("-b", "--boundary", dest="boundary", action="store_true", default=False,
                    help="Export boundary shapes instead of center-lines")
    op.add_argument("-t", "--traffic-lights", action="store_true", default=False, dest="tls",
                    help="Export traffic light geometries")
    op.add_argument("--edgedata-timeline", action="store_true", default=False, dest="edgedataTimeline",
                    help="Exports all time intervals (by default only the first is exported)")
    op.add_argument("-x", "--extra-attributes", action="store_true", default=False, dest="extraAttributes",
                    help="Exports extra attributes from edge and lane "
                         "(such as max speed, number of lanes and allowed vehicles)")

    options = op.parse_args()
    if not options.edges and not options.lanes:
        options.edges = True

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


def addFeature(options, features, addLanes):
    geomType = 'lane' if addLanes else 'edge'
    for id, geometry, width in net.getGeometries(addLanes, options.junctionCoords):
        feature = {"type": "Feature"}
        feature["properties"] = {
            "element": geomType,
            "id": id,
        }
        edgeID = net.getLane(id).getEdge().getID() if addLanes else id
        if edgeID in edgeData:
            if options.edgedataTimeline:
                feature["properties"]["edgeData"] = edgeData[edgeID]
            else:
                feature["properties"].update(edgeData[edgeID][0])

        if edgeID in ptLines:
            for ptType, lines in ptLines[edgeID].items():
                feature["properties"][ptType] = " ".join(sorted(lines))

        if not addLanes or not options.edges:
            feature["properties"]["name"] = net.getEdge(edgeID).getName()
        if options.extraAttributes:
            feature["properties"]["maxSpeed"] = net.getEdge(edgeID).getSpeed()
            if geomType == 'lane':
                feature["properties"]["allow"] = ','.join(sorted(net.getLane(id).getPermissions()))
                feature["properties"]["index"] = net.getLane(id).getIndex()
                outgoingLanes = [lane.getID() for lane in net.getLane(id).getOutgoingLanes()]
                feature["properties"]["outgoingLanes"] = ','.join(sorted(outgoingLanes))
                directions = [conn.getDirection() for conn in net.getLane(id).getOutgoing()]
                feature["properties"]["directions"] = ','.join(sorted(set(directions)))
            else:
                feature["properties"]["numLanes"] = net.getEdge(edgeID).getLaneNumber()
                permissions_union = set()
                for lane in net.getEdge(edgeID).getLanes():
                    permissions_union.update(lane.getPermissions())
                feature["properties"]["allow"] = ",".join(sorted(permissions_union))
                feature["properties"]["fromNode"] = net.getEdge(edgeID).getFromNode().getID()
                feature["properties"]["toNode"] = net.getEdge(edgeID).getToNode().getID()
        if options.boundary:
            geometry = gh.line2boundary(geometry, width)
        feature["geometry"] = shape2json(net, geometry, options.boundary)
        features.append(feature)


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

    if options.edges:
        addFeature(options, features, False)
    if options.lanes:
        addFeature(options, features, True)

    if options.junctions:
        for junction in net.getNodes():
            feature = {"type": "Feature"}
            feature["properties"] = {
                "element": 'junction',
                "id": junction.getID(),
            }
            feature["geometry"] = shape2json(net, junction.getShape(), options.boundary)
            features.append(feature)

    if options.tls:
        for edge in net.getEdges():
            for lane in edge.getLanes():
                nCons = len(lane.getOutgoing())
                for i, con in enumerate(lane.getOutgoing()):
                    if con.getTLSID() != "":
                        feature = {"type": "Feature"}
                        feature["properties"] = {
                            "element": 'tls_connection',
                            "id": "%s_%s" % (con.getJunction().getID(), con.getJunctionIndex()),
                            "tls": con.getTLSID(),
                            "tlIndex": con.getTLLinkIndex(),
                        }
                        barLength = lane.getWidth() / nCons
                        offset = i * barLength - lane.getWidth() * 0.5
                        prev, end = lane.getShape()[-2:]
                        geometry = [gh.add(end, gh.sideOffset(prev, end, offset)),
                                    gh.add(end, gh.sideOffset(prev, end, offset + barLength))]
                        if options.boundary:
                            geometry = gh.line2boundary(geometry, 0.2)
                        feature["geometry"] = shape2json(net, geometry, options.boundary)
                        features.append(feature)

    geojson = {}
    geojson["type"] = "FeatureCollection"
    geojson["features"] = features
    with sumolib.openz(options.outFile, 'w') as outf:
        json.dump(geojson, outf, sort_keys=True, indent=4, separators=(',', ': '))
        print(file=outf)
