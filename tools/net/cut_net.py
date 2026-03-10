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
This script cuts a network according to the first polygon or multilinestring found in the given geojson
"""
import json
import os
import subprocess
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--input-net-file", required=True,
                           category="input", type=argParser.net_file, help="The .net.xml file to cut")
    argParser.add_argument("-j", "--geojson", category="input", required=True,
                           type=argParser.file, help="GeoJSON input file")
    argParser.add_argument("-k", "--netconvert-configuration",
                           category="input", help="Additional netconvert parameter to use")
    argParser.add_argument("-o", "--output-file", category="output",
                           type=argParser.file, help="The network output file name")
    argParser.add_argument("-p", "--polygon-output-file", category="output",
                           type=argParser.file, help="The polygon output file name")
    return argParser.parse_args()


if __name__ == "__main__":
    options = parse_args()
    with open(options.geojson) as jsonf:
        geo = json.load(jsonf)
    for feat in geo.get("features", []):
        if feat["geometry"]["type"] in ("Polygon", "MultiLineString"):
            shape = feat["geometry"]["coordinates"][0]
            break
    if not options.output_file:
        options.output_file = options.input_net_file.replace(".net.xml", "_cut.net.xml")

    cmd = [sumolib.checkBinary("netconvert"), "-s", options.input_net_file, "-o", options.output_file,
           "--keep-edges.in-geo-boundary", ",".join(["%s,%s" % (lon,lat) for lon,lat in shape])]
    print(" ".join(cmd))
    if options.netconvert_configuration:
        cmd += ["-c", options.netconvert_configuration]
    print(cmd)
    subprocess.check_call(cmd)
    # subprocess.check_call([sumolib.checkBinary("netconvert"), "test.netccfg"])
    if options.polygon_output_file:
        with open(options.polygon_output_file, 'w') as outf:
            sumolib.xml.writeHeader(outf, root="additional")
            outf.write('   <poly id="cut" shape="%s" color="blue" geo="true" layer="100"/>\n' % (" ".join(["%s,%s" % (lon,lat) for lon,lat in shape])))
            outf.write('</additional>\n')
