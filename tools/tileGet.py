#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    tileGet.py
# @author  Michael Behrisch
# @date    2019-12-11

from __future__ import absolute_import
from __future__ import print_function
from __future__ import division
import math
import os
try:
    # python3
    import urllib.request as urllib
except ImportError:
    import urllib

import optparse

import sumolib  # noqa

MERCATOR_RANGE = 256
MAX_TILE_SIZE = 640
MAPQUEST_TYPES = {"roadmap": "map", "satellite": "sat", "hybrid": "hyb", "terrain": "sat"}

def fromLatLngToPoint(lat, lng) :
    # inspired by https://stackoverflow.com/questions/12507274/how-to-get-bounds-of-a-google-static-map
    x = lng * MERCATOR_RANGE / 360
    siny = math.sin(lat / 180. * math.pi)
    y = 0.5 * math.log((1 + siny) / (1 - siny)) * -MERCATOR_RANGE / (2 * math.pi)
    return x, y


def getZoomWidthHeight(south, west, north, east, maxTileSize):
    center = ((north + south) / 2, (east + west) / 2)
    centerPx = fromLatLngToPoint(*center)
    nePx = fromLatLngToPoint(north, east)
    zoom = 20
    width = (nePx[0] - centerPx[0]) * 2**zoom * 2
    height = (centerPx[1] - nePx[1]) * 2**zoom * 2
    while width > maxTileSize or height > maxTileSize:
        zoom -= 1
        width /= 2
        height /= 2
    return center, zoom, width, height


optParser = optparse.OptionParser()
optParser.add_option("-p", "--prefix", default="tile", help="for output file")
optParser.add_option("-b", "--bbox", help="bounding box to retrieve in geo coordinates west,south,east,north")
optParser.add_option("-t", "--tiles", type="int",
                     default=1, help="number of tiles the output gets split into")
optParser.add_option("-d", "--output-dir", default=".", help="optional output directory (must already exist)")
optParser.add_option("-s", "--decals-file", default="settings.xml", help="name of decals settings file")
optParser.add_option("-x", "--polygon", help="calculate bounding box from polygon data in file")
optParser.add_option("-n", "--net", help="calculate bounding box from net file")
optParser.add_option("-k", "--key", help="API key to use")
optParser.add_option("-m", "--maptype", default="satellite", help="map type (roadmap, satellite, hybrid, terrain)")
optParser.add_option("-u", "--url", default="maps.googleapis.com/maps/api/staticmap",
                     help="Download from the given tile server")
# alternatives: open.mapquestapi.com/staticmap/v4/getmap


def get(args=None):
    options, _ = optParser.parse_args(args=args)
    if not options.bbox and not options.net and not options.polygon:
        optParser.error("At least one of 'bbox' and 'net' and 'polygon' has to be set.")
    bbox = ((0,0),(0,0))
    if options.polygon:
        west = 1e400
        south = 1e400
        east = -1e400
        north = -1e400
        for area in sumolib.output.parse_fast(options.polygon, 'poly', ['shape']):
            coordList = [tuple(map(float, x.split(',')))
                         for x in area.shape.split()]
            for point in coordList:
                west = min(point[0], west)
                south = min(point[1], south)
                east = max(point[0], east)
                north = max(point[1], north)
    if options.bbox:
        west, south, east, north = [float(v) for v in options.bbox.split(',')]
        if south > north or west > east:
            optParser.error("Invalid geocoordinates in bbox.")
    if options.net:
        net = sumolib.net.readNet(options.net)
        bbox = net.getBBoxXY()
        west, south = net.convertXY2LonLat(*bbox[0])
        east, north = net.convertXY2LonLat(*bbox[1])

    prefix = os.path.join(options.output_dir, options.prefix)
    mapQuest = "mapquest" in options.url
    with open(os.path.join(options.output_dir, options.decals_file), "w") as decals:
        sumolib.xml.writeHeader(decals, root="viewsettings")
        b = west
        for i in range(options.tiles):
            e = b + (east - west) / options.tiles
            offset = (bbox[1][0] - bbox[0][0]) / options.tiles
            c, z, w, h = getZoomWidthHeight(south, b, north, e, 2560 if mapQuest else 640)
            if mapQuest:
                size = "size=%d,%d" % (w, h)
                maptype = 'imagetype=png&type=' + MAPQUEST_TYPES[options.maptype]
            else:
                size = "size=%dx%d" % (w, h)
                maptype = 'maptype=' + options.maptype
            request = "https://%s?%s&center=%.6f,%.6f&zoom=%s&%s&key=%s" % (
                      options.url, size, c[0], c[1], z, maptype, options.key)
#            print(request)
            urllib.urlretrieve(request, "%s%s.png" % (prefix, i))
            print('    <decal filename="%s%s.png" centerX="%s" centerY="%s" width="%s" height="%s"/>'
                  % (prefix, i, bbox[0][0] + (i + 0.5) * offset, (bbox[0][1] + bbox[1][1]) / 2,
                     offset, bbox[1][1] - bbox[0][1]), file=decals)
            b = e
        print("</viewsettings>", file=decals)


if __name__ == "__main__":
    get()
