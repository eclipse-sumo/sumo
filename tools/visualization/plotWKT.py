#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2024-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plotWKT.py
# @author  Benjamin Coueraud
# @date    2024-04-04


import os
import sys

from matplotlib.pyplot import figure, plot, gca, show, savefig
from shapely import wkt
from shapely.geometry import Polygon, MultiPolygon

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

from sumolib.options import ArgumentParser


def plotPolygonWithHoles(polygon, options):
    x, y = polygon.exterior.coords.xy
    plot(x, y, color=options.color)
    for hole in polygon.interiors:
        if Polygon(hole).area > options.area_threshold:
            x, y = hole.coords.xy
            plot(x, y, color=options.color)


def plotMultiPolygonWithHoles(multipolygon, options):
    for polygon in multipolygon.geoms:
        plotPolygonWithHoles(polygon, options)


def main(args=None):
    ap = ArgumentParser(description="Plot a polygon, or the difference between two polygons, given in WKT format.")
    ap.add_argument('filename', category='input', type=ap.file, help='Name of a WKT file')
    ap.add_argument('-f', '--other-filename', category='input', type=ap.file, help='Name of another WKT file')
    ap.add_argument('-o', '--output', category='output', type=ap.file, help='Name of image file to write')
    ap.add_argument('-a', '--area-threshold', default=0.01, type=float,
                    help='Area threshold used to filter small holes')
    ap.add_argument('-m', '--color', default='blue', help='Color used to draw the polygons')
    options = ap.parse_args(args=args)

    with open(options.filename) as file:
        polygon = wkt.load(file)
        if options.other_filename:
            with open(options.other_filename) as otherFile:
                otherPolygon = wkt.load(otherFile)
                polygon = polygon.difference(otherPolygon)

    figure()
    if type(polygon) == Polygon:
        plotPolygonWithHoles(polygon, options)
    elif type(polygon) == MultiPolygon:
        plotMultiPolygonWithHoles(polygon, options)
    gca().set_aspect('equal')
    if options.output:
        savefig(options.output)
    else:
        show()


if __name__ == "__main__":
    main()
