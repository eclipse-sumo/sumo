#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2023 German Aerospace Center (DLR) and others.
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
import numpy as np
import ezdxf


sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa


WALKABLE_COLOR = "red"
OBSTACLE_COLOR = "blue"


def create_test_dxf(args):
    doc = ezdxf.new(dxfversion='R2000')
    msp = doc.modelspace()
    doc.layers.new(name=args.walkable_layer)
    msp.add_lwpolyline(((0,0), (10,0), (10,10), (0,10)), dxfattribs={'layer': args.walkable_layer})
    msp.add_lwpolyline(((100,100), (110,100), (110,110), (100,110)), dxfattribs={'layer': args.walkable_layer})
    doc.layers.new(name=args.obstacle_layer)
    msp.add_lwpolyline(((5,5), (8,5), (8,8), (5,8)), dxfattribs={'layer': args.obstacle_layer})
    doc.saveas(args.file)


def polygon_as_XML_element(polygon, typename, index, color):
    poly = " ".join(["%.2f,%.2f" % c[:2] for c in polygon])
    return '    <poly id="%s_%s" type="%s" color="%s" shape="%s"/>\n' % (typename[9:], index, typename, color, poly)


def generate_circle_vertices(center, radius, nbr_vertices=20):
    angles = np.linspace(0., 2.0*np.pi, nbr_vertices)
    vertices = [(center[0] + radius*np.cos(a), center[1] + radius*np.sin(a)) for a in angles]
    return vertices


def main():
    parser = sumolib.options.ArgumentParser()
    parser.add_argument('file', help='The DXF file to read from')
    parser.add_argument("-o", "--output", help="Name of the polygon output file")
    parser.add_argument("--test", action="store_true", help="Write DXF test file and exit")
    parser.add_argument("--walkable-layer", default="walkable_areas",
                        help="Name of the DXF layer containing walkable areas")
    parser.add_argument("--obstacle-layer", default="obstacles",
                        help="Name of the DXF layer containing obstacles")
    args = parser.parse_args()
    if args.test:
        create_test_dxf(args)
        return

    if args.output is None:
        args.output = args.file[:-3] + "add.xml"
    dxf = ezdxf.readfile(args.file)
    with sumolib.openz(args.output, "w") as add:
        sumolib.xml.writeHeader(add, root="additional", options=args)
        
        msp = dxf.modelspace()
        
        for entity in msp.query("LWPOLYLINE"):
            vertices = list(entity.vertices())
            if entity.dxf.layer == args.walkable_layer:
                add.write(polygon_as_XML_element(vertices, "jupedsim.walkable_area", entity.dxf.handle, WALKABLE_COLOR))
            elif entity.dxf.layer == args.obstacle_layer:
                add.write(polygon_as_XML_element(vertices, "jupedsim.obstacle", entity.dxf.handle, OBSTACLE_COLOR))
            else:
                warnings.warn("Polygon '%s' belonging to unknown layer '%s'." % (entity.dxf.handle, entity.dxf.layer))
                
        for entity in msp.query("CIRCLE"):
            if entity.dxf.layer != args.obstacle_layer:
                warnings.warn("Circle '%s' belongs to layer '%s' instead of layer '%s'." 
                              % (entity.dxf.handle, entity.dxf.layer, args.obstacle_layer))
            else:
                vertices = generate_circle_vertices(list(entity.dxf.center), entity.dxf.radius)
                add.write(polygon_as_XML_element(vertices, "jupedsim.obstacle", entity.dxf.handle, OBSTACLE_COLOR))
                
        add.write("</additional>")


if __name__ == "__main__":
    try:
        main()
    except IOError as e:
        print("Input file is not a DXF file or generic I/O error occured: %s" % e, file=sys.stderr)
        sys.exit(1)
    except ezdxf.DXFStructureError as e:
        print("Invalid or corrupted input DXF file: %s" % e, file=sys.stderr)
        sys.exit(2)
