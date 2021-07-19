#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    osmGet.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2009-08-01

from __future__ import absolute_import
from __future__ import print_function
import os
try:
    import httplib
    import urlparse
    from urllib2 import urlopen
except ImportError:
    # python3
    import http.client as httplib
    import urllib.parse as urlparse
    from urllib.request import urlopen

import base64
from os import path

import sumolib  # noqa
import gzip


def readCompressed(conn, urlpath, query, filename):
    conn.request("POST", "/" + urlpath, """
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


optParser = sumolib.options.ArgumentParser(description="Get network from OpenStreetMap")
optParser.add_argument("-p", "--prefix", default="osm", help="for output file")
optParser.add_argument("-b", "--bbox", help="bounding box to retrieve in geo coordinates west,south,east,north")
optParser.add_argument("-t", "--tiles", type=int,
                       default=1, help="number of tiles the output gets split into")
optParser.add_argument("-d", "--output-dir", help="optional output directory (must already exist)")
optParser.add_argument("-a", "--area", type=int, help="area id to retrieve")
optParser.add_argument("-x", "--polygon", help="calculate bounding box from polygon data in file")
optParser.add_argument("-u", "--url", default="www.overpass-api.de/api/interpreter",
                       help="Download from the given OpenStreetMap server")
# alternatives: overpass.kumi.systems/api/interpreter, sumo.dlr.de/osm/api/interpreter
optParser.add_argument("-w", "--wikidata", action="store_true", dest="wikidata",
                       default=False, help="get the corresponding wikidata")


def get(args=None):
    options = optParser.parse_args(args=args)
    if not options.bbox and not options.area and not options.polygon:
        optParser.error("At least one of 'bbox' and 'area' and 'polygon' has to be set.")
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

    if options.output_dir:
        options.prefix = path.join(options.output_dir, options.prefix)

    if "http" in options.url:
        url = urlparse.urlparse(options.url)
    else:
        url = urlparse.urlparse("https://" + options.url)
    if os.environ.get("https_proxy") is not None:
        headers = {}
        proxy_url = urlparse.urlparse(os.environ.get("https_proxy"))
        if proxy_url.username and proxy_url.password:
            auth = '%s:%s' % (proxy_url.username, proxy_url.password)
            headers['Proxy-Authorization'] = 'Basic ' + base64.b64encode(auth)
        conn = httplib.HTTPSConnection(proxy_url.hostname, proxy_url.port)
        conn.set_tunnel(url.hostname, 443, headers)
    else:
        if url.scheme == "https":
            conn = httplib.HTTPSConnection(url.hostname, url.port)
        else:
            conn = httplib.HTTPConnection(url.hostname, url.port)

    if options.area:
        if options.area < 3600000000:
            options.area += 3600000000
        readCompressed(conn, url.path, '<area-query ref="%s"/>' %
                       options.area, options.prefix + "_city.osm.xml")
    if options.bbox or options.polygon:
        if options.tiles == 1:
            readCompressed(conn, url.path, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' %
                           (north, south, west, east), options.prefix + "_bbox.osm.xml")
        else:
            num = options.tiles
            b = west
            for i in range(num):
                e = b + (east - west) / float(num)
                readCompressed(conn, url.path, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' % (
                    north, south, b, e), "%s%s_%s.osm.xml" % (options.prefix, i, num))
                b = e

    conn.close()
    # extract the wiki data according to the wikidata-value in the extracted osm file
    if options.wikidata:
        filename = options.prefix + '.wikidata.xml.gz'
        osmFile = path.join(os.getcwd(), options.prefix + "_bbox.osm.xml")
        codeSet = set()
        # deal with invalid characters
        bad_chars = [';', ':', '!', "*", ')', '(', '-', '_', '%', '&', '/', '=', '?', '$', '//', '\\', '#', '<', '>']
        for line in open(osmFile, encoding='utf8'):
            subSet = set()
            if 'wikidata' in line and line.split('"')[3][0] == 'Q':
                basicData = line.split('"')[3]
                for i in bad_chars:
                    basicData = basicData.replace(i, ' ')
                elems = basicData.split(' ')
                for e in elems:
                    if e and e[0] == 'Q':
                        subSet.add(e)
                codeSet.update(subSet)

        # make and save query results iteratively
        codeList = list(codeSet)
        interval = 50  # the maximal number of query items
        outf = gzip.open(path.join(os.getcwd(), filename), "wb")
        for i in range(0, len(codeSet), interval):
            j = i + interval
            if j > len(codeSet):
                j = len(codeSet)
            subList = codeList[i:j]
            content = urlopen("https://www.wikidata.org/w/api.php?action=wbgetentities&ids=%s&format=json" %
                              ("|".join(subList))).read()
            print(type(content))
            outf.write(content + b"\n")
        outf.close()


if __name__ == "__main__":
    get()
