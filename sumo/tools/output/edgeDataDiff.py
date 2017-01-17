#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    edgeDataDiff.py
@author  Jakob Erdmann
@date    2015-08-14
@version $Id$

Compute differences between two edgeData-output files

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse
from sumolib.miscutils import uMax, Statistics


def write_diff(orig, new, out):

    diffStats = defaultdict(Statistics)

    with open(out, 'w') as f:
        f.write("<meandata>\n")
        for interval_old, interval_new in zip(parse(orig, 'interval'), parse(new, 'interval')):
            f.write('    <interval begin="%s" end="%s">\n' %
                    (interval_old.begin, interval_old.end))
            for edge_old, edge_new in zip(interval_old.edge, interval_new.edge):
                assert(edge_old.id == edge_new.id)
                f.write('    <edge id="%s"' % edge_old.id)
                for attr in edge_old._fields:
                    if attr == 'id':
                        continue
                    try:
                        delta = float(getattr(edge_new, attr)) - \
                            float(getattr(edge_old, attr))
                        diffStats[attr].add(delta, edge_old.id)
                        f.write(' %s="%s"' % (attr, delta))
                    except:
                        pass
                f.write("/>\n")
            f.write("</interval>\n")

        f.write("</meandata>\n")
        for attr, stats in diffStats.items():
            stats.label = attr
            print(stats)


if __name__ == "__main__":
    try:
        orig, new, out = sys.argv[1:]
    except ValueError:
        print("USAGE: %s <edgedata1.xml> <edgedata2.xml> <output_diff.xml>")
        sys.exit()
    write_diff(orig, new, out)
