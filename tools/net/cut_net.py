#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    cut_net.py
# @author  Michael Behrisch
# @date    2026-03-06

"""
Cut a network using a boundary from a GeoJSON file or a uMap link.
"""
import json
import os
import re
import subprocess
import sys
from urllib.parse import urlsplit, urlunsplit
from urllib.request import urlopen

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--input-net-file", required=True,
                           category="input", type=argParser.net_file, help="The .net.xml file to cut")
    source = argParser.add_mutually_exclusive_group(required=True)
    source.add_argument("-j", "--geojson", category="input", type=argParser.file, help="GeoJSON input file")
    source.add_argument("--umap-url", category="input", help="Public uMap view or share link")
    argParser.add_argument("-k", "--netconvert-configuration",
                           category="input", help="Additional netconvert parameter to use")
    argParser.add_argument("-o", "--output-file", category="output",
                           type=argParser.file, help="The network output file name")
    argParser.add_argument("-p", "--polygon-output-file", category="output",
                           type=argParser.file, help="The polygon output file name")
    return argParser.parse_known_args()


def read_umap(url):
    parts = urlsplit(url)
    if parts.scheme not in ("http", "https") or not parts.netloc:
        raise ValueError("Expected an HTTP or HTTPS uMap URL.")
    if re.fullmatch(r"/m/[0-9]+/?", parts.path):
        with urlopen(url, timeout=30) as response:
            parts = urlsplit(response.geturl())
    match = re.fullmatch(r"(?:/[^/]+)?/map/[^/]+_([0-9]+)/?", parts.path)
    if not match or parts.scheme not in ("http", "https"):
        raise ValueError("Expected a uMap view link ending in /map/<name>_<id> or a uMap short share link.")
    download_url = urlunsplit((parts.scheme, parts.netloc, "/map/%s/download/" % match.group(1), "", ""))
    with urlopen(download_url, timeout=30) as response:
        return json.load(response)


def iter_features(data):
    if not isinstance(data, dict):
        raise ValueError("Expected a GeoJSON object or a uMap export.")
    yield from data.get("features", [])
    for layer in data.get("layers", []):
        yield from iter_features(layer)


def get_boundary(data):
    for feature in iter_features(data):
        geometry = feature.get("geometry") or {}
        if geometry.get("type") in ("Polygon", "MultiLineString"):
            try:
                shape = geometry["coordinates"][0]
                if shape:
                    return [(point[0], point[1]) for point in shape]
            except (KeyError, IndexError, TypeError):
                raise ValueError("Invalid coordinates in the boundary.")
    raise ValueError("No Polygon or MultiLineString boundary found.")


if __name__ == "__main__":
    options, nc_args = parse_args()
    try:
        if options.umap_url:
            geo = read_umap(options.umap_url)
        else:
            with open(options.geojson, encoding="utf-8") as jsonf:
                geo = json.load(jsonf)
        shape = get_boundary(geo)
    except (OSError, ValueError) as e:
        sys.exit("Error: %s" % e)
    if not options.output_file:
        directory, filename = os.path.split(options.input_net_file)
        output_name = filename.replace(".net.xml", "_cut.net.xml")
        if output_name == filename:
            output_name += "_cut.net.xml"
        options.output_file = os.path.join(directory, output_name)

    cmd = [sumolib.checkBinary("netconvert"), "-s", options.input_net_file, "-o", options.output_file,
           "--keep-edges.in-geo-boundary", ",".join(["%s,%s" % (lon, lat) for lon, lat in shape])] + nc_args
    if options.netconvert_configuration:
        cmd += ["-c", options.netconvert_configuration]
    subprocess.check_call(cmd)
    if options.polygon_output_file:
        with open(options.polygon_output_file, 'w') as outf:
            sumolib.xml.writeHeader(outf, root="additional")
            outf.write('   <poly id="cut" shape="%s" color="blue" geo="true" layer="100"/>\n' %
                       (" ".join(["%s,%s" % (lon, lat) for lon, lat in shape])))
            outf.write('</additional>\n')
