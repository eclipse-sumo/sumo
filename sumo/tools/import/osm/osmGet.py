#!/usr/bin/env python
"""
@file    osmGet.py
@author  Daniel.Krajzewicz@dlr.de
@date    2009-08-01
@version $Id$

Retrieves an area from OpenStreetMap.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os, sys, httplib, StringIO, gzip, optparse

def readCompressed(response, query, filename):
    conn.request("POST", "/api/interpreter", """<osm-script timeout="180" element-limit="20000000">
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
    print response.status, response.reason
    if response.status == 200:
        stream = StringIO.StringIO(response.read())
        decompressor = gzip.GzipFile(fileobj=stream)
        out = open(os.path.join(os.path.dirname(sys.argv[0]), filename), "w")
        out.write(decompressor.read())
        out.close()

optParser = optparse.OptionParser()
optParser.add_option("-p", "--prefix", default="osm", help="for output file")
optParser.add_option("-b", "--bbox", help="bounding box to retrieve in geo coordinates south,west,north,east")
optParser.add_option("-o", "--oldapi", action="store_true",
                     default=False, help="use old API for retrieval")
optParser.add_option("-t", "--tiles", type="int", default=1, help="number of tiles when using old api")
optParser.add_option("-a", "--area", type="int", help="area id to retrieve")
(options, args) = optParser.parse_args()

if not options.bbox and not options.area:
    optParser.error("At least one of 'bbox' and 'area' has to be set.")
if options.oldapi and options.area:
    optParser.error("Only the new API supports 'area'.")
if options.bbox:
    south,west,north,east = [float(v) for v in options.bbox.split(',')]
    if south > north or west > east:
        optParser.error("Invalid geocoordinates in bbox.")
if options.oldapi:
    num = options.tiles
    b = west
    conn = httplib.HTTPConnection("api.openstreetmap.org")
    for i in range(num):
        e = b + (east-west) / float(num)
        req = "/api/0.6/map?bbox=%s,%s,%s,%s" % (b, south, e, north)
        conn.request("GET", req)
        r = conn.getresponse()
        print req, r.status, r.reason
        out = open(os.path.join(os.path.dirname(sys.argv[0]), "%s%s_%s.osm.xml" % (options.prefix, i, num)), "w")
        out.write(r.read())
        out.close()
        b = e
    conn.close()
else:
    conn = httplib.HTTPConnection("78.46.81.38")
    if options.area:
        if options.area < 3600000000:
            options.area += 3600000000
        readCompressed(conn, '<area-query ref="%s"/>' % options.area, options.prefix + "_city.osm.xml")
    if options.bbox:
        readCompressed(conn, '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' % (north, south, west, east), options.prefix + "_bbox.osm.xml")
    conn.close()
