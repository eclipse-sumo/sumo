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

# @file    edgeDataStatistics.py
# @author  Johannes Rummel
# @date    2022-04-05

"""Reads a list of edge data files and calculates statistic
values for related attributes.
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
import optparse
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse  # noqa
from sumolib.miscutils import Statistics  # noqa


def mylist_callback(option, opt, value, parser):
    setattr(parser.values, option.dest, value.replace(" ", "").replace('"', '').split(','))


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("--attributes-list", type='string', action='callback', callback=mylist_callback,
                         help="if given, calculate statistic values only for the given attributes")
#    optParser.add_option("--geh", action="store_true",
#                         default=False, help="write geh value instead of absolute differences")
#    optParser.add_option("--undefined", type="float", default=-1001,
#                         help="value to use if the difference is undefined")
#    optParser.add_option("--no-statistics", action="store_true", default=False,
#                         help="otherwise: handle attributes starting with 'std_' "
#                              "as standard deviation and calculate propagated error")
    (options, args) = optParser.parse_args(args=args)

    if len(args) == 2:
        options.input_list, options.out = args
    else:
        optParser.print_help()
        sys.exit(1)

    return options


def write_stats(options):
    """Calculates the statistical values (as mean values and standard deviations) of all (numerical)
    attributes of the meandata files and writes them as a new meandata file
    to the output file."""
    parsed_input_files = []
    for input_file in options.input_list.replace(" ", "").replace('"', '').split(','):
        parsed_file = parse(input_file, 'interval', heterogeneous=True)
        parsed_input_files.append(parsed_file)

    with open(options.out, 'w') as f:
        f.write("<meandata>\n")
        for interval_all in zip(*parsed_input_files):
            # separate calculation for each interval
            f.write('    <interval begin="%s" end="%s">\n' %
                    (interval_all[0].begin, interval_all[0].end))
            edges = dict()    # set of (edge.id, edge)
            all_edges = list()    # list of dicts, with one dict for each file
            for interval in interval_all:
                my_edges = dict([(e.id, e) for e in interval.edge])
                edges.update(my_edges)
                all_edges.append(my_edges)
#            interval_new_edges = dict([(e.id, e) for e in interval_new.edge]) => "edges"
            for id, edge in edges.items():
                f.write('        <edge id="%s"' % id)
                stats = defaultdict(Statistics)    # separate stats for each edge
                for attr in edge._fields:
                    if attr == 'id':
                        continue
                    if options.attributes_list and attr not in options.attributes_list:
                        continue
                    try:
                        for parsed in all_edges:
                            parsed_edge = parsed.get(id, None)
                            if parsed_edge is None:
                                continue
                            try:
                                val = float(getattr(parsed_edge, attr))
                                stats[attr].add(val, id)
                            except Exception:
                                pass
                        mean, std = stats[attr].meanAndStdDev()
                        median = stats[attr].median()
                        q1, median, q3 = stats[attr].quartiles()
                        # todo: add more statistic values here
                        f.write(' %s="%s"' % ("mean_" + attr, mean))
                        f.write(' %s="%s"' % ("std_" + attr, std))
                        f.write(' %s="%s"' % ("median_" + attr, median))
                        f.write(' %s="%s"' % ("q1_" + attr, q1))
                        f.write(' %s="%s"' % ("q3_" + attr, q3))
                    except Exception:
                        pass
                f.write("/>\n")
            f.write("    </interval>\n")

        f.write("</meandata>\n")


if __name__ == "__main__":
    write_stats(get_options())
