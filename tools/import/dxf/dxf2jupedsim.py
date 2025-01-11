#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2014-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    dxf2jupedsim.py
# @author  Benjamin Coueraud
# @author  Michael Behrisch
# @date    2023-06-16


import os
import sys
import warnings
import itertools

import numpy
import ezdxf
import pyproj
from shapely.geometry import Polygon, LineString

sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa


def create_test_dxf(args):
    doc = ezdxf.new(dxfversion='R2000')
    msp = doc.modelspace()
    doc.layers.new(name=args.walkable_layer)
    msp.add_lwpolyline(((0, 0), (10, 0), (10, 10), (0, 10)), dxfattribs={'layer': args.walkable_layer})
    msp.add_lwpolyline(((100, 100), (110, 100), (110, 110), (100, 110)), dxfattribs={'layer': args.walkable_layer})
    doc.layers.new(name=args.obstacle_layer)
    msp.add_lwpolyline(((5, 5), (8, 5), (8, 8), (5, 8)), dxfattribs={'layer': args.obstacle_layer})
    msp.add_circle((2, 2), 1, dxfattribs={'layer': args.obstacle_layer})
    doc.saveas(args.file)


def polygon_as_XML_element(polygon, typename, index, color, layer):
    polygonID = "jps.%s_%s" % (typename[9:], index)

    # Round the coordinates.
    polygon = [(round(point[0], 9), round(point[1], 9)) for point in polygon]

    # Check for equal consecutive points.
    cleanPolygon = [polygon[0]]
    duplicates = []
    for i in range(1, len(polygon)):
        if polygon[i][0] != polygon[i-1][0] or polygon[i][1] != polygon[i-1][1]:
            cleanPolygon.append(polygon[i])
        else:
            duplicates.append(polygon[i])
    if len(cleanPolygon) < len(polygon):
        duplicates = " ".join(["%.9f,%.9f" % c[:2] for c in duplicates])
        print("Warning: polygon '%s' had some equal consecutive points removed: %s" % (polygonID, duplicates))

    # Check for simplicity and output hints.
    if not Polygon(cleanPolygon).is_valid:
        print("Warning: polygon '%s' is not simple." % polygonID)
        segments = list(map(LineString, zip(cleanPolygon[:-1], cleanPolygon[1:])))
        intersect = False
        for segment1, segment2 in itertools.combinations(segments, 2):
            if segment1.crosses(segment2):
                intersect = True
                print("Hint: segments [(%.9f, %.9f)] and [(%.9f, %.9f)] intersect each other."
                      % (segment1.coords[0][0], segment1.coords[0][1], segment2.coords[1][0], segment2.coords[1][1]))
        if not intersect:
            duplicates = {point for point in cleanPolygon[:-1] if cleanPolygon[:-1].count(point) > 1}
            for point in duplicates:
                print("Hint: point [(%.9f, %.9f)] appears at least twice." % (point[0], point[1]))

    # Create the XML element.
    poly = " ".join(["%.9f,%.9f" % c[:2] for c in cleanPolygon])
    return ('    <poly id="%s" type="%s" color="%s" fill="True" layer="%s" shape="%s" geo="True"/>\n' %
            (polygonID, typename, color, layer, poly))


def generate_circle_vertices(center, radius, nbr_vertices=20):
    angles = numpy.linspace(0., 2.0*numpy.pi, nbr_vertices)
    vertices = [(center[0] + radius*numpy.cos(a), center[1] + radius*numpy.sin(a)) for a in angles]
    return vertices


def apply_inverse_projection(vertices, projection):
    projection = pyproj.Proj(projection)
    return [projection(vertex[0], vertex[1], inverse=True) for vertex in vertices]


def main():
    parser = sumolib.options.ArgumentParser()
    parser.add_argument('file', help='The DXF file to read from', category="input", type=parser.file)
    parser.add_argument("-o", "--output", help="Name of the polygon output file", category="output")
    parser.add_argument("--test", action="store_true", help="Write DXF test file and exit")
    parser.add_argument("--walkable-layer", default="walkable_areas",
                        help="Name of the DXF layer containing walkable areas")
    parser.add_argument("--obstacle-layer", default="obstacles",
                        help="Name of the DXF layer containing obstacles")
    parser.add_argument("--walkable-color", default="179,217,255",
                        help="Color of the polygons defining walkable areas")
    parser.add_argument("--obstacle-color", default="255,204,204",
                        help="Color of the polygons defining obstacles")
    parser.add_argument("--sumo-layer", default=0,
                        help="SUMO layer used to render polygons defining walkable areas")
    parser.add_argument("--projection", default="EPSG:32633",
                        help="EPSG code or projection string used to convert back to geocoordinates")
    args = parser.parse_args()
    if args.test:
        create_test_dxf(args)
        return

    if args.output is None:
        args.output = args.file[:-3] + "add.xml"
    dxf = ezdxf.readfile(args.file)
    with sumolib.openz(args.output, "w") as add:
        sumolib.xml.writeHeader(add, root="additional", options=args)
        for entity in dxf.modelspace().query("LWPOLYLINE CIRCLE"):
            if entity.dxf.dxftype == "CIRCLE":
                vertices = generate_circle_vertices(list(entity.dxf.center), entity.dxf.radius)
            else:
                vertices = list(entity.vertices())
                if vertices[-1] != vertices[0]:
                    vertices.append(vertices[0])
            if args.projection != "none":
                geoVertices = apply_inverse_projection(vertices, args.projection)
            else:
                geoVertices = vertices
            if entity.dxf.layer == args.walkable_layer:
                add.write(polygon_as_XML_element(geoVertices, "jupedsim.walkable_area", entity.dxf.handle,
                                                 args.walkable_color, args.sumo_layer))
            elif entity.dxf.layer == args.obstacle_layer:
                add.write(polygon_as_XML_element(geoVertices, "jupedsim.obstacle", entity.dxf.handle,
                                                 args.obstacle_color, args.sumo_layer+1))
            else:
                warnings.warn("Polygon '%s' belonging to unknown layer '%s'." % (entity.dxf.handle, entity.dxf.layer))
        add.write("</additional>\n")


if __name__ == "__main__":
    try:
        main()
    except IOError as e:
        print("Input file is not a DXF file or generic I/O error occured: %s" % e, file=sys.stderr)
        sys.exit(1)
    except ezdxf.DXFStructureError as e:
        print("Invalid or corrupted input DXF file: %s" % e, file=sys.stderr)
        sys.exit(2)
