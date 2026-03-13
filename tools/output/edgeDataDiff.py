#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
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
from sumolib.miscutils import Statistics, geh, short_names, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa
from routeSampler import getHourFraction  # noqa


def get_options(args=None):
    optParser = ArgumentParser()
    optParser.add_argument("orig", help="original data file")
    optParser.add_argument("new", help="modified data file")
    optParser.add_argument("out", help="diff output file")
    optParser.add_option("--relative", action="store_true", default=False,
                         help="write relative instead of absolute differences")
    optParser.add_option("--geh", action="store_true", default=False,
                         help="write geh value instead of absolute differences")
    optParser.add_argument("--geh-scale", dest="gehScale", type=float, default=None,
                           help="Should be set to 0.1 when loading traffic for a full day "
                                "(estimating peak hour traffic as 1/10 of daily traffic)")
    optParser.add_option("--undefined", type=float, default=-1001, help="value to use if the difference is undefined")
    optParser.add_option("--attributes", help="compare list of custom attributes (A1,A2,B1,B2,C1,C2,...)")
    optParser.add_option("--no-statistics", action="store_true", default=False,
                         help="otherwise: handle attributes starting with 'std_' as standard"
                         + "deviation and calculate propagated error")
    options = optParser.parse_args(args)
    if options.attributes:
        options.attributes = options.attributes.split(',')
        if len(options.attributes) % 2 != 0:
            print("Option --attributes requires an even-number list of attribute names")
            sys.exit(1)
    return options

def getIntervalTime(i):
    return (parseTime(i.begin), parseTime(i.end))

def write_diff(options):

    diffStats = defaultdict(Statistics)
    attrList = None
    if options.attributes:
        tmp = [[], []]
        for i, a in enumerate(options.attributes):
            tmp[i % 2].append(a)
        attrList = list(zip(*tmp))

    with open(options.out, 'w') as f:
        f.write("<meandata>\n")
        oldIntervals = {}
        unmatched_new = []
        for interval_old in parse(options.orig, 'interval'):
            oldIntervals[getIntervalTime(interval_old)] = interval_old
        for interval_new in parse(options.new, 'interval'):
            time = getIntervalTime(interval_new)
            interval_old = oldIntervals.get(time)
            if interval_old is None:
                unmatched_new.append(time)
                continue
            del oldIntervals[time]
            f.write('    <interval begin="%s" end="%s" id="%s@%s - %s@%s">\n' %
                    (interval_old.begin, interval_old.end,
                        interval_new.id, options.new,
                        interval_old.id, options.orig))
            interval_new_edges = dict([(e.id, e) for e in interval_new.edge]) if interval_new.edge else {}
            hourFraction = getHourFraction(options, *time)
            if interval_old.edge is None:
                continue
            for edge_old in interval_old.edge:
                edge_new = interval_new_edges.get(edge_old.id, None)
                if edge_new is None:
                    continue
                assert edge_old.id == edge_new.id
                f.write('        <edge id="%s"' % edge_old.id)
                if not options.attributes:
                    attrList = zip(edge_old._fields, edge_old._fields)
                for attr, attr2 in attrList:
                    if attr == 'id':
                        continue
                    try:
                        val_new = float(getattr(edge_new, attr2))
                        val_old = float(getattr(edge_old, attr))
                        delta = val_new - val_old
                        if not options.no_statistics and attr.startswith('std_'):
                            delta = math.sqrt(val_new**2 + val_old**2)
                        else:
                            fmt = ' %s="%s"'
                            if options.relative:
                                if val_old != 0:
                                    delta /= abs(val_old)
                                else:
                                    delta = options.undefined
                            elif options.geh:
                                delta = geh(val_new / hourFraction, val_old / hourFraction)
                                fmt = ' %s="%.2f"'
                        diffStats[attr].add(delta, edge_old.id)
                        f.write(fmt % (attr, delta))
                    except Exception:
                        pass
                f.write("/>\n")
            f.write("    </interval>\n")

        f.write("</meandata>\n")
        if unmatched_new:
            print("%s intervals from %s where not found %s (earliest %s, latest %s)" % (
                len(unmatched_new), options.new, options.orig, unmatched_new[0], unmatched_new[-1]),
                file=sys.stderr)
        unmatched_old = list(oldIntervals.keys())
        if unmatched_old:
            print("%s intervals from %s where not found %s (earliest %s, latest %s)" % (
                len(unmatched_old), options.orig, options.new, unmatched_old[0], unmatched_old[-1]),
                file=sys.stderr)

        for attr, stats in diffStats.items():
            stats.label = attr
            if not options.geh:
                stats.abs = True
            print(stats)


if __name__ == "__main__":
    write_diff(get_options())
