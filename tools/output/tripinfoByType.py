#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    tripinfoByType.py
# @author  Jakob Erdmann
# @date    2021-11-19

"""
Aggregate tripinfo statistics by vehicle type.
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse  # noqa
from sumolib.miscutils import Statistics, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa


def get_options(args=None):
    argParser = ArgumentParser()
    argParser.add_argument("-t", "--tripinfo-file", dest="tripinfoFile",
                           help="tripinfo file written by the simulation")
    argParser.add_argument("-a", "--attribute", default="duration",
                           help="attribute to use for statistics")
    argParser.add_argument("-o", "--output", help="the output file")
    argParser.add_argument("-i", "--interval", help="custom aggregation interval (seconds or H:M:S)")
    argParser.add_argument("--by-arrivals", action="store_true", default=False, dest="byArrivals",
                           help="When using --interval, aggregated by arrival time instead of depart time")
    options = argParser.parse_args(args=args)
    if not options.tripinfoFile:
        sys.exit("Required argument --tripinfo-file is missing")

    if options.interval is not None:
        options.interval = parseTime(options.interval)
    return options


def getAggregatedTime(options, elem):
    if options.interval:
        val = elem.arrival if options.byArrivals else elem.depart
        return int(parseTime(val) / options.interval) * options.interval
    else:
        return None


def main(options):
    intervals = defaultdict(dict)  # time -> (type -> stats)

    for trip in parse(options.tripinfoFile, 'tripinfo'):
        typeStats = intervals[getAggregatedTime(options, trip)]
        if trip.vType not in typeStats:
            typeStats[trip.vType] = Statistics(trip.vType)
        typeStats[trip.vType].add(parseTime(getattr(trip, options.attribute)), trip.id)

    for person in parse(options.tripinfoFile, 'personinfo'):
        for stage in person.getChildList():
            if stage.hasAttribute(options.attribute):
                typeStats = intervals[getAggregatedTime(options, stage)]
                if stage.name not in typeStats:
                    typeStats[stage.name] = Statistics(stage.name)
                typeStats[stage.name].add(parseTime(getattr(stage, options.attribute)), person.id)

    if options.output:
        with open(options.output, 'w') as outf:
            sumolib.writeXMLHeader(outf, "$Id$", "tripinfosByType", options=options)  # noqa
            for time in sorted(intervals.keys()):
                typeStats = intervals[time]
                if time is not None:
                    outf.write('  <interval begin="%s" end="%s">\n' % (time, time + options.interval))
                for vType, stats in sorted(typeStats.items()):
                    q1, median, q3 = stats.quartiles()
                    outf.write('    <typeInfo vType="%s" count="%s" min="%s" minVeh="%s"' %
                               (vType, stats.count(), stats.min, stats.min_label))
                    outf.write(' max="%s" maxVeh="%s" mean="%s" Q1="%s" median="%s" Q3="%s"/>\n' %
                               (stats.max, stats.max_label, stats.avg(), q1, median, q3))
                if time is not None:
                    outf.write('  </interval>\n')
            outf.write('</tripinfosByType>\n')
    else:
        for time in sorted(intervals.keys()):
            typeStats = intervals[time]
            if time is not None:
                print("Interval: [%s, %s[" % (time, time + options.interval))
            for vType, stats in sorted(typeStats.items()):
                print(stats)


if __name__ == "__main__":
    options = get_options()
    main(options)
