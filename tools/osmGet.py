#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
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
import sys
import os
import gzip
import base64
import ssl
import json
import collections

try:
    import httplib
    import urlparse
    from urllib2 import urlopen
except ImportError:
    # python3
    import http.client as httplib
    import urllib.parse as urlparse
    from urllib.request import urlopen
try:
    import certifi
    HAVE_CERTIFI = True
except ImportError:
    HAVE_CERTIFI = False

import sumolib

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
TYPEMAP_DIR = os.path.join(THIS_DIR, "..", "data", "typemap")


def readCompressed(options, conn, urlpath, query, roadTypesJSON, getShapes, filename):
    # generate query string for each road-type category
    queryStringNode = []

    commonQueryStringNode = """
    <query type="nwr">
            %s
            %s
        </query>"""

    for category in roadTypesJSON:
        if len(roadTypesJSON[category]) > 0:
            typeList = "|".join(roadTypesJSON[category])
            regvQueryString = '<has-kv k="%s" modv="" regv="%s"/>' % (category, typeList)
            queryStringNode.append(commonQueryStringNode % (regvQueryString, query))

    if getShapes and roadTypesJSON:
        keyValueDict = collections.defaultdict(set)
        for typemap in ["osmPolyconvert.typ.xml", "osmPolyconvertRail.typ.xml"]:
            with open(os.path.join(TYPEMAP_DIR, typemap), 'r') as osmPolyconvert:
                for polygon in sumolib.xml.parse(osmPolyconvert, 'polygonType'):
                    keyValue = polygon.id.split('.') + ["."]
                    keyValueDict[keyValue[0]].add(keyValue[1])

        for category, value in keyValueDict.items():
            if category in roadTypesJSON:
                continue
            if '.' in value:
                regvQueryString = '<has-kv k="%s"/>' % category
            else:
                typeList = "|".join(value)
                regvQueryString = '<has-kv k="%s" modv="" regv="%s"/>' % (category, typeList)
            queryStringNode.append(commonQueryStringNode % (regvQueryString, query))

    if queryStringNode:
        # include all nodes that are relevant for public transport
        regvQueryString = '<has-kv k="public_transport" modv="" regv="."/>'
        queryStringNode.append(commonQueryStringNode % (regvQueryString, query))

    if queryStringNode and getShapes:
        unionQueryString = """
    <union into="nodesBB">
      %s
    </union>
    <union into="waysBB">
       %s
    </union>
    <union into="waysBB2">
       <item from="waysBB"/>
       <recurse type="way-node"/>
       <recurse type="node-relation"/>
       <recurse type="way-relation"/>
     </union>
     <union into="waysBB3">
       <item from="waysBB2"/>
       <recurse type="relation-way"/>
       <recurse type="way-node"/>
    </union>
    <query type="node">
       <item from="nodesBB"/>
       <item from="waysBB3"/>
    </query>
    <query type="way" into="waysBB4">
       <item from="waysBB3"/>
    </query>
    <query type="relation" into="relsBB4">
       <item from="waysBB3"/>
    </query>
   <union>
     <item/>
     <item from="waysBB4"/>
     <item from="relsBB4"/>
   </union>
    """ % (query, "\n".join(queryStringNode))

    elif queryStringNode:
        unionQueryString = """
    <union>
       %s
    </union>
    <union>
       <item/>
       <recurse type="way-node"/>
       <recurse type="node-relation"/>
       <recurse type="way-relation"/>
    </union>""" % "\n".join(queryStringNode)

    else:
        unionQueryString = """
    <union>
       %s
       <recurse type="node-relation" into="rels"/>
       <recurse type="node-way"/>
       <recurse type="way-relation"/>
     </union>
     <union>
        <item/>
        <recurse type="way-node"/>
     </union>""" % query

    finalQuery = """
    <osm-script timeout="240" element-limit="1073741824">
       %s
    <print mode="body"/>
    </osm-script>""" % unionQueryString

    if options.query_output:
        with open(options.query_output, "w") as outf:
            outf.write(finalQuery)

    conn.request("POST", "/" + urlpath, finalQuery, headers={'Accept-Encoding': 'gzip'})

    response = conn.getresponse()
    if options.verbose:
        print(response.status, response.reason)
    if response.status == 200:
        with open(filename, "wb") as out:
            if response.getheader('Content-Encoding') == 'gzip':
                lines = gzip.decompress(response.read())
            else:
                lines = response.read()
            declClose = lines.find(b'>') + 1
            lines = (lines[:declClose]
                     + b"\n"
                     + sumolib.xml.buildHeader(options=options).encode()
                     + lines[declClose:])
            if filename.endswith(".gz"):
                out.write(gzip.compress(lines))
            else:
                out.write(lines)


def get_options(args):
    optParser = sumolib.options.ArgumentParser(description="Get network from OpenStreetMap")
    optParser.add_argument("-p", "--prefix", category="output", default="osm", help="for output file")
    optParser.add_argument("-b", "--bbox", category="input",
                           help="bounding box to retrieve in geo coordinates west,south,east,north")
    optParser.add_argument("-t", "--tiles", type=int,
                           default=1, help="number of tiles the output gets split into")
    optParser.add_argument("-d", "--output-dir", category="output",
                           help="optional output directory (must already exist)")
    optParser.add_argument("-a", "--area", type=int, help="area id to retrieve")
    optParser.add_argument("-x", "--polygon", category="processing",
                           help="calculate bounding box from polygon data in file")
    optParser.add_argument("-u", "--url", default="www.overpass-api.de/api/interpreter",
                           help="Download from the given OpenStreetMap server")
    # alternatives: overpass.kumi.systems/api/interpreter, sumo.dlr.de/osm/api/interpreter
    optParser.add_argument("-w", "--wikidata", action="store_true",
                           default=False, help="get the corresponding wikidata")
    optParser.add_argument("-r", "--road-types", dest="roadTypes",
                           help="only delivers osm data to the specified road-types")
    optParser.add_argument("-s", "--shapes", action="store_true", default=False,
                           help="determines if polygon data (buildings, areas , etc.) is downloaded")
    optParser.add_argument("-z", "--gzip", category="output", action="store_true",
                           default=False, help="save gzipped output")
    optParser.add_argument("-q", "--query-output", category="output",
                           help="write query to the given FILE")
    optParser.add_argument("-v", "--verbose", action="store_true",
                           default=False, help="tell me what you are doing")
    options = optParser.parse_args(args=args)
    if not options.bbox and not options.area and not options.polygon:
        optParser.error("At least one of 'bbox' and 'area' and 'polygon' has to be set.")
    if options.bbox:
        west, south, east, north = [float(v) for v in options.bbox.split(',')]
        if south > north or west > east or south < -90 or north > 90 or west < -180 or east > 180:
            optParser.error("Invalid geocoordinates in bbox.")
    return options


def get(args=None):
    options = get_options(args)
    if options.polygon:
        west = 1e400
        south = 1e400
        east = -1e400
        north = -1e400
        for area in sumolib.output.parse_fast(options.polygon, 'poly', ['shape']):
            for coord in area.shape.split():
                point = tuple(map(float, coord.split(',')))
                west = min(point[0], west)
                south = min(point[1], south)
                east = max(point[0], east)
                north = max(point[1], north)
    if options.bbox:
        west, south, east, north = [float(v) for v in options.bbox.split(',')]

    if options.output_dir:
        options.prefix = os.path.join(options.output_dir, options.prefix)

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
            context = ssl.create_default_context(cafile=certifi.where() if HAVE_CERTIFI else None)
            context.minimum_version = ssl.TLSVersion.TLSv1_2
            conn = httplib.HTTPSConnection(url.hostname, url.port, context=context)
        else:
            conn = httplib.HTTPConnection(url.hostname, url.port)

    roadTypesJSON = json.loads(options.roadTypes.replace("\'", "\"").lower()) if options.roadTypes else {}

    suffix = ".osm.xml.gz" if options.gzip else ".osm.xml"
    if options.area:
        if options.area < 3600000000:
            options.area += 3600000000
        readCompressed(options, conn, url.path, '<area-query ref="%s"/>' %
                       options.area, roadTypesJSON, options.shapes, options.prefix + "_city" + suffix)
    if options.bbox or options.polygon:
        if options.tiles == 1:
            readCompressed(options, conn, url.path, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' %
                           (north, south, west, east), roadTypesJSON,
                           options.shapes,
                           options.prefix + "_bbox" + suffix)
        else:
            num = options.tiles
            b = west
            for i in range(num):
                if options.verbose:
                    print("Getting tile %d of %d." % (i + 1, num))
                e = b + (east - west) / float(num)
                readCompressed(options, conn, url.path, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' % (
                    north, south, b, e), roadTypesJSON, options.shapes,
                    "%s%s_%s%s" % (options.prefix, i, num, suffix))
                b = e

    conn.close()
    # extract the wiki data according to the wikidata-value in the extracted osm file
    if options.wikidata:
        filename = options.prefix + '.wikidata.xml.gz'
        osmFile = options.prefix + "_bbox" + suffix
        codeSet = set()
        # deal with invalid characters
        bad_chars = [';', ':', '!', "*", ')', '(', '-', '_', '%', '&', '/', '=', '?', '$', '//', '\\', '#', '<', '>']
        for line in sumolib.openz(osmFile):
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
        outf = gzip.open(filename, "wb")
        for i in range(0, len(codeSet), interval):
            j = i + interval
            if j > len(codeSet):
                j = len(codeSet)
            subList = codeList[i:j]
            content = urlopen("https://www.wikidata.org/w/api.php?action=wbgetentities&ids=%s&format=json" %
                              ("|".join(subList))).read()
            if options.verbose:
                print(type(content))
            outf.write(content + b"\n")
        outf.close()


if __name__ == "__main__":
    try:
        get()
    except ssl.CertificateError:
        print("Error with SSL certificate, try 'pip install -U certifi'.", file=sys.stderr)
