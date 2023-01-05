#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    net2poly.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2022-03-06

"""
This script converts the edge geometries of a sumo network to an additional file with polygons
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " -n <net> <options>"
    argParser = sumolib.options.ArgumentParser(usage=USAGE)
    argParser.add_argument("-n", "--net-file", dest="netFile", required=True,
                           help="The .net.xml file to convert")
    argParser.add_argument("-o", "--output-file", dest="outFile", help="The polygon output file name")
    argParser.add_argument("-l", "--lanes", action="store_true", default=False,
                           help="Export lane geometries instead of edge geometries")
    argParser.add_argument("-i", "--internal", action="store_true", default=False,
                           help="Export internal geometries")
    argParser.add_argument("--layer", default="10", help="Layer for normal edges")
    argParser.add_argument("--color", default="red", help="Color for normal edges")
    argParser.add_argument("--internal-color", dest="iColor", default="orange", help="Color for internal edges")
    return argParser.parse_args()


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.netFile, withInternal=options.internal)
    geomType = 'lane' if options.lanes else 'edge'

    with open(options.outFile, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="additional")
        for id, geometry, width in net.getGeometries(options.lanes):
            color = options.iColor if id[0] == ":" else options.color
            shape = ["%.6f,%.6f" % net.convertXY2LonLat(x, y) for x, y in geometry]
            outf.write('    <poly id="%s" color="%s" layer="%s" lineWidth="%s" shape="%s" geo="1"/>\n' %
                       (id, color, options.layer, width, " ".join(shape)))
        outf.write('</additional>\n')
