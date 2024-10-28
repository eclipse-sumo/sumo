#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    circlePolygon.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Yun-Pang Floetteroed
# @date    2010-02-20


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import math
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))
import sumolib  # noqa


def setCircle(idx, x, y, r, c, prefix, type, color, fill, layer, output):
    angle = 2 * math.pi / c
    shape = ""
    for i in range(c):
        shape += "%.2f,%.2f " % (math.cos(i * angle) * r + x,
                                 math.sin(i * angle) * r + y)
        if i == 0:
            beginPoint = shape
    shape += beginPoint
    print('    <poly id="%s%s" type="%s" color="%s" fill="%i" layer="%s" shape="%s"/>' % (
        prefix, idx, type, color, fill, layer, shape[:-1]),
        file=output)


if __name__ == "__main__":

    op = sumolib.options.ArgumentParser(description="Make a circle polygon")

    op.add_option("files", nargs='+', category="input", type=op.file_list,
                  help="List of XML files to plot")
    op.add_option("-r", "--radius", type=float, default=100,
                  help="default radius")
    op.add_option("-p", "--prefix", default="poly",
                  help="id prefix")
    op.add_option("-t", "--type", default="unknown",
                  help="type string")
    op.add_option("--color", default="1,0,0",
                  help="color string")
    op.add_option("-f", "--fill", action="store_true",
                  default=False, help="fill the polygons")
    op.add_option("-l", "--layer", type=float, default=-1,
                  help="layer")
    op.add_option("-x", "--corners", type=int, default=100,
                  help="default number of corners")
    op.add_option("-o", "--output-file", category="output", type=op.file,
                  help="output file (default: standard output)")

    try:
        options = op.parse_args()
    except (NotImplementedError, ValueError) as e:
        print(e, file=sys.stderr)
        sys.exit(1)

    output = sys.stdout if options.output_file is None else open(options.output_file, 'w')
    print("<additional>", file=output)
    for idx, d in enumerate(options.files):
        desc = d.split(",")
        x = float(desc[0])
        y = float(desc[1])
        r = float(desc[2]) if len(desc) > 2 else options.radius
        c = int(desc[3]) if len(desc) > 3 else options.corners

        setCircle(idx, x, y, r, c, options.prefix, options.type, options.color, options.fill, options.layer, output)
    print("</additional>", file=output)
