#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    osmGet.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2009-08-01

from __future__ import absolute_import
from __future__ import print_function
import os
try:
    # python3
    import urllib.request as urllib
except ImportError:
    import urllib

import optparse

import sumolib  # noqa


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
# alternatives: ???


def get(args=None):
    (options, args) = optParser.parse_args(args=args)
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
    print ("retrieving", west, south, east, north)
    size_x = size_y = 320
    scale = 2
    with open(os.path.join(options.output_dir, options.decals_file), "w") as decals:
        sumolib.xml.writeHeader(decals, root="viewsettings")
        if options.tiles == 1:
            urllib.urlretrieve("http://%s?size=%dx%d&visible=%.6f,%.6f|%.6f,%.6f&scale=%s&maptype=%s&key=%s"
                               % (options.url, size_x, size_y, south, west, north, east,
                                  scale, options.maptype, options.key), prefix+".png")
            print('    <decal filename="%s.png" centerX="%s" centerY="%s" width="%s" height="%s"/>'
                  % (prefix, (bbox[0][0] + bbox[1][0]) / 2, (bbox[0][1] + bbox[1][1]) / 2,
                     (bbox[1][0] - bbox[0][0]) / scale, (bbox[1][1] - bbox[0][1]) / scale), file=decals)
        else:
            num = options.tiles
            b = west
            for i in range(num):
                e = b + (east - west) / float(num)
                urllib.urlretrieve("http://%s?size=%dx%d&visible=%.6f,%.6f|%.6f,%.6f&scale=%s&maptype=%s&key=%s"
                                   % (options.url, size_x, size_y, south, west, north, east,
                                      scale, options.maptype, options.key), "%s%s_%s.png" % (prefix, i, num))
                b = e
                print('    <decal filename="%s%s_%s.png" centerX="%s" centerY="%s" width="%s" height="%s"/>'
                      % (prefix, i, num, (bbox[0][0] + bbox[1][0]) / 2, (bbox[0][1] + bbox[1][1]) / 2,
                         (bbox[1][0] - bbox[0][0]) / scale, (bbox[1][1] - bbox[0][1]) / scale), file=decals)
        print("</viewsettings>", file=decals)


if __name__ == "__main__":
    get()