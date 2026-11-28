#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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
import xml.etree.ElementTree as ET
import ssl
import json
import collections
import time

try:
    from urllib2 import urlopen, Request
    HTTPError = URLError = Exception
except ImportError:
    # python3
    from urllib.request import urlopen, Request
    from urllib.error import HTTPError, URLError
try:
    import certifi
    HAVE_CERTIFI = True
except ImportError:
    HAVE_CERTIFI = False
import sumolib

DEBUG = False
if DEBUG:
    import urllib
    opener = urllib.request.build_opener(
        urllib.request.HTTPHandler(debuglevel=1),
        urllib.request.HTTPSHandler(debuglevel=1)
    )
    urllib.request.install_opener(opener)

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
TYPEMAP_DIR = os.path.join(THIS_DIR, "..", "data", "typemap")

EUROPE = [
    (-25.0, 66.8),
    (15.0, 71.5),
    (32.0, 70.5),
    (41.0, 60.0),
    (41.0, 43.0),
    (28.0, 34.5),
    (14.5, 35.5),
    (-6.0, 35.8),
    (-10.5, 36.8),
    (-11.5, 51.5),
    (-25.0, 63.0)]

NORTH_AMERICA = [
    (-168.0, 66.0),
    (-168.0, 52.0),
    (-138.0, 58.0),
    (-126.0, 48.0),
    (-118.0, 32.0),
    (-106.0, 20.0),
    (-84.0, 8.0),
    (-77.0, 7.5),
    (-72.0, 18.0),
    (-64.0, 18.0),
    (-79.5, 25.5),
    (-66.0, 44.0),
    (-52.0, 47.0),
    (-60.0, 62.0),
    (-85.0, 74.0),
    (-140.0, 71.0)]


def clip_segment_to_bbox(p0, p1, west, south, east, north):
    x0, y0 = p0
    x1, y1 = p1
    dx = x1 - x0
    dy = y1 - y0
    t0, t1 = 0.0, 1.0
    for p, q in ((-dx, x0 - west), (dx, east - x0), (-dy, y0 - south), (dy, north - y0)):
        if p == 0:
            if q < 0:
                return None
        else:
            r = q / float(p)
            if p < 0:
                if r > t1:
                    return None
                if r > t0:
                    t0 = r
            else:
                if r < t0:
                    return None
                if r < t1:
                    t1 = r
    return ((x0 + t0 * dx, y0 + t0 * dy), (x0 + t1 * dx, y0 + t1 * dy), t0 > 0.0, t1 < 1.0)


def cut_ways_at_bbox(osm_bytes, west, south, east, north):
    root = ET.fromstring(osm_bytes)
    nodes = {}
    node_elems = {}
    for n in root.findall("node"):
        nid = n.get("id")
        if nid is not None and n.get("lon") is not None and n.get("lat") is not None:
            nodes[nid] = (float(n.get("lon")), float(n.get("lat")))
            node_elems[nid] = n

    cut_idx = 0
    for way in list(root.findall("way")):
        wid = way.get("id", "way")
        nds = [nd.get("ref") for nd in way.findall("nd") if nd.get("ref") in nodes]
        tags = [ET.Element("tag", dict(t.attrib)) for t in way.findall("tag")]
        pieces = []
        cur = []
        for u, v in zip(nds, nds[1:]):
            clipped = clip_segment_to_bbox(nodes[u], nodes[v], west, south, east, north)
            if clipped is None:
                if len(cur) >= 2:
                    pieces.append(cur)
                cur = []
                continue
            q0, q1, c0, c1 = clipped
            if c0:
                u_ref = "%s_cut_%d" % (wid, cut_idx)
                cut_idx += 1
                nodes[u_ref] = q0
                ne = ET.Element("node", {"id": u_ref, "lon": "%.7f" % q0[0], "lat": "%.7f" % q0[1]})
                root.insert(0, ne)
                node_elems[u_ref] = ne
            else:
                u_ref = u
            if c1:
                v_ref = "%s_cut_%d" % (wid, cut_idx)
                cut_idx += 1
                nodes[v_ref] = q1
                ne = ET.Element("node", {"id": v_ref, "lon": "%.7f" % q1[0], "lat": "%.7f" % q1[1]})
                root.insert(0, ne)
                node_elems[v_ref] = ne
            else:
                v_ref = v
            if not cur:
                cur = [u_ref, v_ref]
            else:
                if cur[-1] != u_ref:
                    if len(cur) >= 2:
                        pieces.append(cur)
                    cur = [u_ref, v_ref]
                elif cur[-1] != v_ref:
                    cur.append(v_ref)
            if c1:
                if len(cur) >= 2:
                    pieces.append(cur)
                cur = []
        if len(cur) >= 2:
            pieces.append(cur)

        root.remove(way)
        for idx, piece in enumerate(pieces):
            new_id = wid if len(pieces) == 1 else "%s#%d" % (wid, idx)
            new_attrs = dict(way.attrib)
            new_attrs["id"] = new_id
            w_el = ET.Element("way", new_attrs)
            for ref in piece:
                w_el.append(ET.Element("nd", {"ref": ref}))
            for t_el in tags:
                w_el.append(ET.Element("tag", dict(t_el.attrib)))
            root.append(w_el)

    referenced = set()
    for way in root.findall("way"):
        for nd in way.findall("nd"):
            referenced.add(nd.get("ref"))
    for rel in root.findall("relation"):
        for mem in rel.findall("member"):
            if mem.get("type") == "node":
                referenced.add(mem.get("ref"))
    for nid, (lon, lat) in list(nodes.items()):
        if nid not in referenced and not (west <= lon <= east and south <= lat <= north):
            if nid in node_elems:
                root.remove(node_elems[nid])

    return b'<?xml version="1.0" encoding="UTF-8"?>\n' + ET.tostring(root, encoding="utf-8")


def readCompressed(options, urls, context, query, roadTypesJSON, getShapes, filename, bbox=None):
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
    <union into="waysBB">
        %s
    </union>
    <union into="nodesBB">
        %s
        <item from="waysBB"/>
        <recurse type="way-node"/>
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
    """ % ("\n".join(queryStringNode), query)

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

    finalQuery = """<osm-script timeout="240" element-limit="1073741824">
        %s
    <print mode="body"/>
</osm-script>""" % unionQueryString

    if options.query_output:
        with open(options.query_output, "w") as outf:
            outf.write(finalQuery)

    headers = {"Accept-Encoding": "gzip",
               "Content-Type": "application/xml",
               "User-Agent": "Eclipse SUMO osmGet.py (sumo@dlr.de)",
               "Referer": "https://eclipse.dev/sumo/"
               }
    for idx in range(options.retries + 1):
        url = urls[idx % len(urls)]
        try:
            if options.verbose:
                print("querying url:", url)
            req = Request(url, data=finalQuery.encode(), headers=headers)
            with urlopen(req, context=context, timeout=options.timeout) as response:
                if options.verbose:
                    print(response.status, response.reason)
                if response.getheader('Content-Encoding') == 'gzip':
                    lines = gzip.decompress(response.read())
                else:
                    lines = response.read()
                # validate before injecting SUMO header
                if b"<osm" not in lines[:1000]:
                    raise ValueError("Server %s returned non-OSM content" % url)
                if options.cut_ways and bbox is not None:
                    lines = cut_ways_at_bbox(lines, *bbox)
                declClose = lines.find(b'>') + 1
                lines = (lines[:declClose]
                         + b"\n"
                         + sumolib.xml.buildHeader(options=options).encode()
                         + lines[declClose:])
                with open(filename, "wb") as out:
                    if filename.endswith(".gz"):
                        out.write(gzip.compress(lines))
                    else:
                        out.write(lines)
                break
        except HTTPError as e:
            if idx < options.retries:
                if e.code in [429, 502, 503, 504]:
                    if options.verbose:
                        print("Download from %s failed (%s %s), retrying in %ds."
                              % (url, e.code, e.msg, options.retry_delay))
                    time.sleep(options.retry_delay)
                    continue
                urls = [u for u in urls if u != url]
                if urls:
                    print("Removing %s from list of valid servers (%s %s)." % (url, e.code, e.msg))
                    continue
            raise
        except (ValueError, URLError):
            urls = [u for u in urls if u != url]
            if urls:
                print("Removing %s from list of valid servers." % url)
                continue
            raise


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
    optParser.add_argument("-u", "--url", default="dressel,dressel-na,hpi,oapi,pcoffee",
                           help="Download from the given Overpass server(s)")
    optParser.add_argument("-w", "--wikidata", action="store_true",
                           default=False, help="get the corresponding wikidata")
    optParser.add_argument("-r", "--road-types", dest="roadTypes",
                           help="only delivers osm data to the specified road-types")
    optParser.add_argument("-s", "--shapes", action="store_true", default=False,
                           help="determines if polygon data (buildings, areas, etc.) is downloaded")
    optParser.add_argument("--cut-ways", dest="cut_ways", category="processing", action="store_true",
                           default=False, help="cut OSM ways at the configured bounding box")
    optParser.add_argument("-z", "--gzip", category="output", action="store_true",
                           default=False, help="save gzipped output")
    optParser.add_argument("-q", "--query-output", category="output",
                           help="write query to the given FILE")
    optParser.add_argument("--config-output", category="output",
                           help="write configuration to the given FILE and not as comment into the output file")
    optParser.add_argument("--retries", type=int, default=5,
                           help="How many attempts to download if a 504 is encountered")
    optParser.add_argument("--timeout", type=int, default=250,
                           help="Set request timeout")
    optParser.add_argument("--retry-delay", type=int, default=3, help="Delay between download attempts")
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

    shortcut = {"oapi": "https://overpass-api.de/api/interpreter",
                "hpi": "https://osm.hpi.de/overpass/api/interpreter",
                "pcoffee": "https://overpass.private.coffee/api/interpreter",
                "vkmaps": "https://maps.mail.ru/osm/tools/overpass/api/interpreter",
                "dressel-na": "https://ovp.dresel.systems/api-na/interpreter",
                "dressel": "https://ovp.dresel.systems/api/interpreter"}

    restricted = {"dressel": EUROPE, "dressel-na": NORTH_AMERICA}
    urlopts = options.url.split(",")
    for u, shape in restricted.items():
        if u in urlopts:
            if options.area:
                urlopts.remove(u)
            else:
                for pos in [(west, south), (west, north), (east, north), (east, south)]:
                    if not sumolib.geomhelper.isWithin(pos, shape):
                        urlopts.remove(u)
                        break

    urls = [shortcut.get(u, u) for u in urlopts]
    context = ssl.create_default_context(cafile=certifi.where() if HAVE_CERTIFI else None)
    context.minimum_version = ssl.TLSVersion.TLSv1_2

    roadTypesJSON = json.loads(options.roadTypes.replace("\'", "\"").lower()) if options.roadTypes else {}

    suffix = ".osm.xml.gz" if options.gzip else ".osm.xml"
    if options.area:
        if options.area < 3600000000:
            options.area += 3600000000
        readCompressed(options, urls, context, '<area-query ref="%s"/>' %
                       options.area, roadTypesJSON, options.shapes, options.prefix + "_city" + suffix)
    if options.bbox or options.polygon:
        if options.tiles == 1:
            readCompressed(options, urls, context,
                           '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' % (north, south, west, east),
                           roadTypesJSON, options.shapes, options.prefix + "_bbox" + suffix,
                           bbox=(west, south, east, north))
        else:
            num = options.tiles
            b = west
            for i in range(num):
                if options.verbose:
                    print("Getting tile %d of %d." % (i + 1, num))
                e = b + (east - west) / float(num)
                readCompressed(options, urls, context,
                               '<bbox-query n="%s" s="%s" w="%s" e="%s"/>' % (north, south, b, e),
                               roadTypesJSON, options.shapes, "%s%s_%s%s" % (options.prefix, i, num, suffix),
                               bbox=(b, south, e, north))
                b = e

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
