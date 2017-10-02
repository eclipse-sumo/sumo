#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    circlePolygon.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2010-02-20
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import sys
import math
from optparse import OptionParser

optParser = OptionParser()
optParser.add_option("-r", "--radius", type="float", default=100,
                     help="default radius")
optParser.add_option("-p", "--prefix", default="poly",
                     help="id prefix")
optParser.add_option("-t", "--type", default="unknown",
                     help="type string")
optParser.add_option("-c", "--color", default="1,0,0",
                     help="color string")
optParser.add_option("-f", "--fill", action="store_true",
                     default=False, help="fill the polygons")
optParser.add_option("-l", "--layer", type="int", default=-1,
                     help="layer")
optParser.add_option("-x", "--corners", type="int", default=100,
                     help="default number of corners")
optParser.add_option(
    "-o", "--output-file", help="output file (default: standard output)")
(options, args) = optParser.parse_args()

output = sys.stdout if options.output_file is None else open(
    options.output_file, 'w')

if len(args) == 0:
    print >> sys.stderr, "Usage: " + sys.argv[0] + " x,y[[,r],c] ..."
    sys.exit()


print("<additional>", file=output)
for idx, d in enumerate(args):
    desc = d.split(",")
    x = float(desc[0])
    y = float(desc[1])
    r = float(desc[2]) if len(desc) > 2 else options.radius
    c = int(desc[3]) if len(desc) > 3 else options.corners
    angle = 2 * math.pi / c
    shape = ""
    for i in range(c):
        shape += "%.2f,%.2f " % (math.cos(i * angle) * r + x,
                                 math.sin(i * angle) * r + y)
    print('    <poly id="%s%s" type="%s" color="%s" fill="%i" layer="%s" shape="%s"/>' % (
        options.prefix, idx, options.type, options.color, options.fill, options.layer, shape[:-1]),
        file=output)
print("</additional>", file=output)
