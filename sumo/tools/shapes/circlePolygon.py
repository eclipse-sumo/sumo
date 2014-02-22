#!/usr/bin/env python
"""
@file    poi_atTLS.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-02-20
@version $Id$

Approximates a list of circles by polygons.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys, math
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
(options, args) = optParser.parse_args()

if len(args) == 0:
    print >> sys.stderr, "Usage: " + sys.argv[0] + " x,y[[,r],c] ..."
    sys.exit()

print "<additional>"
for idx, d in enumerate(args):
    desc = d.split(",")
    x = float(desc[0])
    y = float(desc[1])
    r = float(desc[2]) if len(desc) > 2 else options.radius
    c = int(desc[3]) if len(desc) > 3 else options.corners
    angle = 2 * math.pi / c
    shape = ""
    for i in range(c):
        shape += "%s,%s " % (math.cos(i * angle) * r + x, math.sin(i * angle) * r + y)
    print '    <poly id="%s%s" type="%s" color="%s"' % (options.prefix, idx, options.type, options.color),
    print 'fill="%i" layer="%s" shape="%s"/>' % (options.fill, options.layer, shape[:-1])
print "</additional>"
