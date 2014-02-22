#!/usr/bin/env python
"""
@file    randomTrips.py
@author  Jakob Erdmann
@date    2012-10-11
@version $Id$

Extract all used edges from routes and person plans and output a file suitable
for pruning edges with netconvert

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,sys
from collections import defaultdict
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.output import parse, parse_fast

route_file, keep_file = sys.argv[1:]
edges = set()
for route in parse_fast(route_file, 'route', ['edges']):
    edges.update(route.edges.split())
for walk in parse_fast(route_file, 'walk', ['edges']):
    edges.update(walk.edges.split())

with open(keep_file, 'w') as outf:
    outf.write(','.join(edges) + '\n')
