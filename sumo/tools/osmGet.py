#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    osmGet.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2009-08-01
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import os
try:
    import httplib
    import urlparse
except ImportError:
    # python3
    import http.client as httplib
    import urllib.parse as urlparse

import optparse
from os import path

import sumolib  # noqa

def readCompressed(conn, query, filename):
    conn.request("POST", "/api/interpreter", """
    <osm-script timeout="240" element-limit="1073741824">
    <union>
       %s
       <recurse type="node-relation" into="rels"/>
       <recurse type="node-way"/>
       <recurse type="way-relation"/>
    </union>
    <union>
       <item/>
       <recurse type="way-node"/>
    </union>
    <print mode="body"/>
    </osm-script>""" % query)
    response = conn.getresponse()
    print(response.status, response.reason)
    if response.status == 200:
        out = open(path.join(os.getcwd(), filename), "wb")
        out.write(response.read())
        out.close()

optParser = optparse.OptionParser()
optParser.add_option("-p", "--prefix", default="osm", help="for output file")
optParser.add_option(
    "-b", "--bbox", help="bounding box to retrieve in geo coordinates south,west,north,east")
optParser.add_option("-o", "--oldapi", action="store_true",
                     default=False, help="use old API for retrieval")
optParser.add_option("-t", "--tiles", type="int",
                     default=1, help="number of tiles when using old api")
optParser.add_option(
    "-d", "--output-dir", help="optional output directory (must already exist)")
optParser.add_option("-a", "--area", type="int", help="area id to retrieve")
optParser.add_option(
    "-x", "--polygon", help="calculate bounding box from polygon data in file")


def get(args=None):
    (options, args) = optParser.parse_args(args=args)
    if not options.bbox and not options.area and not options.polygon:
        optParser.error(
            "At least one of 'bbox' and 'area' and 'polygon' has to be set.")
    if options.oldapi and options.area:
        optParser.error("Only the new API supports 'area'.")
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
        south, west, north, east = [float(v) for v in options.bbox.split(',')]
        if south > north or west > east:
            optParser.error("Invalid geocoordinates in bbox.")

    if options.output_dir:
        options.prefix = path.join(options.output_dir, options.prefix)

    if options.oldapi:
        num = options.tiles
        b = west
        conn = httplib.HTTPConnection("api.openstreetmap.org")
        for i in range(num):
            e = b + (east - west) / float(num)
            req = "/api/0.6/map?bbox=%s,%s,%s,%s" % (b, south, e, north)
            conn.request("GET", req)
            r = conn.getresponse()
            print(req, r.status, r.reason)
            out = open(
                path.join(os.getcwd(), "%s%s_%s.osm.xml" % (options.prefix, i, num)), "w")
            out.write(r.read())
            out.close()
            b = e
        conn.close()
    else:
        host = 'www.overpass-api.de'
        #host= 'overpass.osm.rambler.ru'
        port = 443
        
        if os.environ.get("https_proxy") is not None:
            proxy_url = os.environ.get("https_proxy")
            url = urlparse.urlparse(proxy_url)
            conn = httplib.HTTPSConnection(url.hostname, url.port)
            conn.set_tunnel(host, port, {})
        else:
            conn = httplib.HTTPConnection(host)

        if options.area:
            if options.area < 3600000000:
                options.area += 3600000000
            readCompressed(conn, '<area-query ref="%s"/>' %
                           options.area, options.prefix + "_city.osm.xml")
        if options.bbox or options.polygon:
            if options.tiles == 1:
                readCompressed(conn, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' %
                               (north, south, west, east), options.prefix + "_bbox.osm.xml")
            else:
                num = options.tiles
                b = west
                for i in range(num):
                    e = b + (east - west) / float(num)
                    readCompressed(conn, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' % (
                        north, south, b, e), "%s%s_%s.osm.xml" % (options.prefix, i, num))
                    b = e

        conn.close()

if __name__ == "__main__":
    get()
