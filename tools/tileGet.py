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

# @file    tileGet.py
# @author  Michael Behrisch
# @author  Robert Hilbrich
# @date    2019-12-11

from __future__ import absolute_import
from __future__ import print_function
from __future__ import division
import math
import os
import sys
from multiprocessing.pool import Pool
import signal

try:
    # python3
    import urllib.request as urllib
    from urllib.error import HTTPError as urlerror
except ImportError:
    import urllib
    from urllib2 import HTTPError as urlerror

import sumolib  # noqa

MERCATOR_RANGE = 256
MAX_TILE_SIZE = 640
MAPQUEST_TYPES = {"roadmap": "map", "satellite": "sat", "hybrid": "hyb", "terrain": "sat"}


def fromLatLonToPoint(lat, lon):
    # inspired by https://stackoverflow.com/questions/12507274/how-to-get-bounds-of-a-google-static-map
    x = lon * MERCATOR_RANGE / 360
    siny = math.sin(math.radians(lat))
    y = 0.5 * math.log((1 + siny) / (1 - siny)) * -MERCATOR_RANGE / (2 * math.pi)
    return x, y


def fromLatLonToTile(lat, lon, zoom):
    # inspired by https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Python
    n = 2.0 ** zoom
    xtile = int((lon + 180.0) / 360.0 * n)
    ytile = int((1.0 - math.asinh(math.tan(math.radians(lat))) / math.pi) / 2.0 * n)
    return xtile, ytile


def fromTileToLatLon(xtile, ytile, zoom):
    n = 2.0 ** zoom
    lon = xtile / n * 360.0 - 180.0
    lat = math.degrees(math.atan(math.sinh(math.pi * (1 - 2 * ytile / n))))
    return lat, lon


def getZoomWidthHeight(south, west, north, east, maxTileSize):
    center = ((north + south) / 2, (east + west) / 2)
    centerPx = fromLatLonToPoint(*center)
    nePx = fromLatLonToPoint(north, east)
    zoom = 20
    width = (nePx[0] - centerPx[0]) * 2**zoom * 2
    height = (centerPx[1] - nePx[1]) * 2**zoom * 2
    while width > maxTileSize or height > maxTileSize:
        zoom -= 1
        width /= 2
        height /= 2
    return center, zoom, width, height


def worker(options, request, filename):
    if options.simulate:
        print(request, filename)
    else:
        urllib.urlretrieve(request, filename)
        if os.stat(filename).st_size < options.min_file_size:
            raise ValueError("small file")


def retrieveOpenStreetMapTiles(options, west, south, east, north, decals, net, is_retina):
    zoom = options.maxZoom + 1
    numTiles = options.tiles + 1
    while numTiles > options.tiles:
        zoom -= 1
        sx, sy = fromLatLonToTile(north, west, zoom)
        ex, ey = fromLatLonToTile(south, east, zoom)
        numTiles = (ex - sx + 1) * (ey - sy + 1)

    if options.user_agent:
        opener = urllib.build_opener()
        opener.addheaders = [('User-agent', options.user_agent)]
        urllib.install_opener(opener)

    for x in range(sx, ex + 1):
        for y in range(sy, ey + 1):
            scale = '@2x' if is_retina and "cartodb" in options.url else ''
            request = "%s/%s/%s/%s%s.png" % (options.url, zoom, x, y, scale)

            filename = os.path.join(options.output_dir, "%s%s_%s.png" % (options.prefix, x, y))
            worker(options, request, filename)
            if net is not None:
                lat, lon = fromTileToLatLon(x, y, zoom)
                upperLeft = net.convertLonLat2XY(lon, lat)
                lat, lon = fromTileToLatLon(x + 0.5, y + 0.5, zoom)
                center = net.convertLonLat2XY(lon, lat)
                print('    <decal file="%s" centerX="%s" centerY="%s" width="%s" height="%s" layer="%d"/>' %
                      (os.path.basename(filename), center[0], center[1],
                       2 * (center[0] - upperLeft[0]), 2 * (upperLeft[1] - center[1]), options.layer), file=decals)


def retrieveMapServerTiles(options, west, south, east, north, decals, net, pattern):
    zoom = 20
    numTiles = options.tiles + 1
    while numTiles > options.tiles:
        zoom -= 1
        sx, sy = fromLatLonToTile(north, west, zoom)
        ex, ey = fromLatLonToTile(south, east, zoom)
        numTiles = (ex - sx + 1) * (ey - sy + 1)

    # opener = urllib.build_opener()
    # opener.addheaders = [('User-agent', 'Mozilla/5.0')]
    # urllib.install_opener(opener)

    if options.parallel_jobs != 0:
        original_sigint_handler = signal.signal(signal.SIGINT, signal.SIG_IGN)
        pool = Pool(options.parallel_jobs)
        signal.signal(signal.SIGINT, original_sigint_handler)

    futures = []
    for x in range(sx, ex + 1):
        for y in range(sy, ey + 1):
            request = options.url + pattern.format(z=zoom, y=y, x=x)
            suffix = ".png" if pattern.endswith(".png") else ".jpeg"
            filename = os.path.join(options.output_dir, "%s%s_%s%s" % (options.prefix, x, y, suffix))
            if options.parallel_jobs == 0:
                worker(options, request, filename)
            else:
                futures.append((x, y, pool.apply_async(worker, (options, request, filename))))
            if net is not None:
                lat, lon = fromTileToLatLon(x, y, zoom)
                upperLeft = net.convertLonLat2XY(lon, lat)
                lat, lon = fromTileToLatLon(x + 0.5, y + 0.5, zoom)
                center = net.convertLonLat2XY(lon, lat)
                print('    <decal file="%s" centerX="%s" centerY="%s" width="%s" height="%s" layer="%d"/>' %
                      (os.path.basename(filename), center[0], center[1],
                       2 * (center[0] - upperLeft[0]), 2 * (upperLeft[1] - center[1]), options.layer), file=decals)
    for x, y, future in futures:
        future.get()


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-p", "--prefix", category="output", default="tile", help="for output file")
    optParser.add_option("-b", "--bbox", category="input",
                         help="bounding box to retrieve in geo coordinates west,south,east,north")
    optParser.add_option("-t", "--tiles", type=int, default=1,
                         help="maximum number of tiles the output gets split into")
    optParser.add_option("-d", "--output-dir", category="output", default=".",
                         help="optional output directory (must already exist)")
    optParser.add_option("-s", "--decals-file", category="output",
                         default="settings.xml", help="name of decals settings file")
    optParser.add_option("-l", "--layer", type=int, default=0,
                         help="(int) layer at which the image will appear, default 0")
    optParser.add_option("-x", "--polygon", category="input", help="calculate bounding box from polygon data in file")
    optParser.add_option("-n", "--net", category="input", help="get bounding box from net file")
    optParser.add_option("-k", "--key", help="API key to use")
    optParser.add_option("-m", "--maptype", default="satellite",
                         help="map type (roadmap, satellite, hybrid, terrain)")
    optParser.add_option("-u", "--url", default="arcgis",
                         help="Download from the given tile server")
    optParser.add_option("-a", "--user-agent",
                         help="user agent string to be used when downloading tiles")
    optParser.add_option("-f", "--min-file-size", type=int, default=3000,
                         help="maximum number of tiles the output gets split into")
    optParser.add_option("--simulate", action="store_true", default=False,
                         help="print download urls and filenames instead of requesting from tile server")
    optParser.add_option("-z", "--max-zoom", type=int, default=17, dest="maxZoom",
                         help="restrict maximum zoom level")
    optParser.add_option("-j", "--parallel-jobs", type=int, default=0,
                         help="Number of parallel jobs to run when downloading tiles. 0 means no parallelism.")
    optParser.add_option("-r", "--retina", action="store_true", default=False,
                         help="set 'true' for double resolution tiles (applies to cartodb only).")

    URL_SHORTCUTS = {
        "arcgis": "https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile",
        "mapquest": "https://www.mapquestapi.com/staticmap/v5/map",
        "google": "https://maps.googleapis.com/maps/api/staticmap",
        "berlin2024": "https://tiles.codefor.de/berlin-2025-dop20rgbi",
        "osm": "https://tile.openstreetmap.org",
        "osm_hot": "https://a.tile.openstreetmap.fr/hot",
        "cartodb_dark": "https://cartodb-basemaps-a.global.ssl.fastly.net/dark_nolabels/",
        "cartodb_light_all": "https://cartodb-basemaps-a.global.ssl.fastly.net/light_all/",
        "cartodb_dark_all": "https://cartodb-basemaps-a.global.ssl.fastly.net/dark_all/",
        "cartodb_light_nolabels": "https://cartodb-basemaps-a.global.ssl.fastly.net/light_nolabels/",
        "cartodb_light_only_labels": "https://cartodb-basemaps-a.global.ssl.fastly.net/light_only_labels/",
        "cartodb_dark_nolabels": "https://cartodb-basemaps-a.global.ssl.fastly.net/dark_nolabels/",
        "cartodb_dark_only_labels": "https://cartodb-basemaps-a.global.ssl.fastly.net/dark_only_labels/"
    }
    options = optParser.parse_args(args=args)
    if not options.bbox and not options.net and not options.polygon:
        optParser.error("At least one of 'bbox' and 'net' and 'polygon' has to be set.")
    options.url = URL_SHORTCUTS.get(options.url.lower(), options.url)
    if not options.url.startswith("http"):
        options.url = "https://" + options.url
    if options.bbox:
        west, south, east, north = [float(v) for v in options.bbox.split(',')]
        if south > north or west > east:
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
            coordList = [tuple(map(float, x.split(',')))
                         for x in area.shape.split()]
            for point in coordList:
                west = min(point[0], west)
                south = min(point[1], south)
                east = max(point[0], east)
                north = max(point[1], north)
    if options.bbox:
        west, south, east, north = [float(v) for v in options.bbox.split(',')]
    net = None
    if options.net:
        net = sumolib.net.readNet(options.net)
        bboxNet = net.getBBoxXY()
        offset = (bboxNet[1][0] - bboxNet[0][0]) / options.tiles
        west, south = net.convertXY2LonLat(*bboxNet[0])
        east, north = net.convertXY2LonLat(*bboxNet[1])

    prefix = os.path.join(options.output_dir, options.prefix)
    mapQuest = "mapquest" in options.url
    with sumolib.openz(os.path.join(options.output_dir, options.decals_file), "w") as decals:
        sumolib.xml.writeHeader(decals, root="viewsettings")
        if "MapServer" in options.url or "berlin" in options.url:
            pattern = "/{z}/{x}/{y}.png" if "berlin" in options.url else "/{z}/{y}/{x}"
            retrieveMapServerTiles(options, west, south, east, north, decals, net, pattern)
        elif "openstreetmap" in options.url or "geofabrik" in options.url or "cartodb" in options.url:
            retrieveOpenStreetMapTiles(options, west, south, east, north, decals, net, options.retina)
        else:
            b = west
            for i in range(options.tiles):
                e = b + (east - west) / options.tiles
                c, z, w, h = getZoomWidthHeight(south, b, north, e, 2560 if mapQuest else 640)
                if mapQuest:
                    size = "size=%d,%d" % (w, h)
                    maptype = 'imagetype=png&type=' + MAPQUEST_TYPES[options.maptype]
                else:
                    size = "size=%dx%d" % (w, h)
                    maptype = 'maptype=' + options.maptype
                request = ("%s?%s&center=%.6f,%.6f&zoom=%s&%s&key=%s" %
                           (options.url, size, c[0], c[1], z, maptype, options.key))
                # print(request)
                filename = os.path.join(options.output_dir, "%s%s.png" % (prefix, i))
                urllib.urlretrieve(request, filename)
                if os.stat(filename).st_size < options.min_file_size:
                    raise ValueError("small file")
                if net is not None:
                    print('    <decal file="%s" centerX="%s" centerY="%s" width="%s" height="%s" layer="%d"/>' %
                          (os.path.basename(filename),
                           bboxNet[0][0] + (i + 0.5) * offset, (bboxNet[0][1] + bboxNet[1][1]) / 2,
                           offset, bboxNet[1][1] - bboxNet[0][1], options.layer), file=decals)
                b = e
        print("</viewsettings>", file=decals)


if __name__ == "__main__":
    try:
        get()
    except urlerror as e:
        print("Error: Tile server returned %s." % e, file=sys.stderr)
        if e.code == 403:
            print(" Maybe an API key is required.", file=sys.stderr)
    except ValueError as e:
        print("Error: Tile server returned %s." % e, file=sys.stderr)
