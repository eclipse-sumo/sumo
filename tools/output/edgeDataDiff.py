#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    edgeDataDiff.py
# @author  Jakob Erdmann
# @author  Johannes Rummel
# @date    2015-08-14

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
import math
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from sumolib.xml import parse  # noqa
from sumolib.miscutils import Statistics, geh  # noqa
from sumolib.options import ArgumentParser  # noqa


def get_options(args=None):
    optParser = ArgumentParser()
    optParser.add_argument("orig", help="original data file")
    optParser.add_argument("new", help="modified data file")
    optParser.add_argument("out", help="diff output file")
    optParser.add_option("--relative", action="store_true", default=False,
                         help="write relative instead of absolute differences")
    optParser.add_option("--geh", action="store_true", default=False,
                         help="write geh value instead of absolute differences")
    optParser.add_option("--undefined", type=float, default=-1001, help="value to use if the difference is undefined")
    optParser.add_option("--no-statistics", action="store_true", default=False,
                         help="otherwise: handle attributes starting with 'std_' as standard"
                         + "deviation and calculate propagated error")
    return optParser.parse_args(args)


def write_diff(options):

    diffStats = defaultdict(Statistics)

    with open(options.out, 'w') as f:
        f.write("<meandata>\n")
        for interval_old, interval_new in zip(
                parse(options.orig, 'interval', heterogeneous=True),
                parse(options.new, 'interval', heterogeneous=True)):
            f.write('    <interval begin="%s" end="%s">\n' %
                    (interval_old.begin, interval_old.end))
            interval_new_edges = dict([(e.id, e) for e in interval_new.edge])
            for edge_old in interval_old.edge:
                edge_new = interval_new_edges.get(edge_old.id, None)
                if edge_new is None:
                    continue
                assert(edge_old.id == edge_new.id)
                f.write('        <edge id="%s"' % edge_old.id)
                for attr in edge_old._fields:
                    if attr == 'id':
                        continue
                    try:
                        val_new = float(getattr(edge_new, attr))
                        val_old = float(getattr(edge_old, attr))
                        delta = val_new - val_old
                        if not options.no_statistics and attr.startswith('std_'):
                            delta = math.sqrt(val_new**2 + val_old**2)
                        else:
                            if options.relative:
                                if val_old != 0:
                                    delta /= abs(val_old)
                                else:
                                    delta = options.undefined
                            elif options.geh:
                                delta = geh(val_new, val_old)
                        diffStats[attr].add(delta, edge_old.id)
                        f.write(' %s="%s"' % (attr, delta))
                    except Exception:
                        pass
                f.write("/>\n")
            f.write("    </interval>\n")

        f.write("</meandata>\n")
        for attr, stats in diffStats.items():
            stats.label = attr
            print(stats)


if __name__ == "__main__":
    write_diff(get_options())
