#!/usr/bin/env python
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

# @file    fcdDiff.py
# @author  Jakob Erdmann
# @date    2022-12-06

"""
Compare two fcd output files with regard to spatial difference
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from math import sqrt

import pandas as pd

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime, humanReadableTime  # noqa
from sumolib.statistics import Statistics  # noqa
from sumolib.xml import parse_fast_nested  # noqa

pd.options.display.width = 0  # auto-detect terminal width

GROUPSTATS = {
    'mean': lambda s: s.avg(),
    'median': lambda s: s.median(),
    'min': lambda s: s.min,
    'max': lambda s: s.max,
}


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Compare route-file stop timing with stop-output")
    parser.add_argument("old", help="the first fcd file")
    parser.add_argument("new", help="the second fcd file")
    parser.add_argument("-o", "--xml-output", dest="output",
                        help="xml output file")
    parser.add_option("--csv-output", dest="csv_output", help="write diff as csv", metavar="FILE")
    parser.add_option("--filter-ids", dest="filterIDs", help="only include data points from the given list of ids")
    parser.add_option("--exclude-ids", dest="excludeIDs", help="skip data points from the given list of ids")
    parser.add_argument("-H", "--human-readable-time", dest="hrTime", action="store_true", default=False,
                        help="Write time values as hour:minute:second or day:hour:minute:second rathern than seconds")
    parser.add_argument("-t", "--tripid", dest="tripId", action="store_true", default=False,
                        help="use attr tripId for matching instead of vehicle id")
    parser.add_argument("-i", "--histogram", dest="histogram", type=float,
                        help="histogram bin size")
    parser.add_argument("--grouped", action="store_true", default=False, help="provide statistics grouped by id")
    parser.add_argument("-v", "--verbose", action="store_true",
                        default=False, help="tell me what you are doing")

    options = parser.parse_args(args=args)
    if options.old is None or options.new is None:
        parser.print_help()
        sys.exit()

    if options.filterIDs is not None:
        options.filterIDs = set(options.filterIDs.split(','))
    if options.excludeIDs is not None:
        options.excludeIDs = set(options.excludeIDs.split(','))

    return options


ATTR_CONVERSIONS = {
    'time': parseTime,
}


def key(row):
    return "%s_%s" % (row[0], humanReadableTime(row['t']))


def getDataFrame(options, fname, attrs, columns, tripId):
    data = []
    if tripId:
        orderedAttrs = attrs[2:] + [attrs[0]]
    else:
        orderedAttrs = [attrs[0]] + attrs[2:]

    for ts, v in parse_fast_nested(fname, 'timestep', ['time'], 'vehicle', orderedAttrs):
        vID = getattr(v, attrs[0])
        if options.excludeIDs is not None and vID in options.excludeIDs:
            continue
        if options.filterIDs is None or vID in options.filterIDs:
            data.append([vID, parseTime(ts.time)] + [float(getattr(v, a)) for a in attrs[2:]])
    return pd.DataFrame.from_records(data, columns=columns)


def main(options):
    idAttr = 'tripId' if options.tripId else 'id'
    attrs = [
        idAttr,
        't',  # time
        'x',
        'y',
    ]
    cols2 = [
        idAttr,
        't',  # time
        'x2',
        'y2',
    ]

    df1 = getDataFrame(options, options.old, attrs, attrs, options.tripId)
    print("read", options.old)
    df2 = getDataFrame(options, options.new, attrs, cols2, options.tripId)
    print("new", options.old)

    # merge on common columns id, time
    df = pd.merge(df1, df2, on=attrs[:2], how="inner")
    # outer merge to count missing entries
    dfOuter = pd.merge(df1, df2, on=attrs[:2], how="outer")

    if options.verbose:
        print("Found %s matches" % len(df))
        print("Found %s records" % len(dfOuter))
        print("missing in old: %s" % dfOuter['x'].isna().sum())
        print("missing in new: %s" % dfOuter['x2'].isna().sum())

    useHist = options.histogram is not None
    s = Statistics("euclidian_error", histogram=useHist, scale=options.histogram)
    idStats = {}

    def euclidian_error(r):
        e = sqrt((r['x'] - r['x2']) ** 2 + (r['y'] - r['y2']) ** 2)
        s.add(e, key(r))
        if options.grouped:
            i = r[idAttr]
            if i not in idStats:
                idStats[i] = Statistics(i, histogram=useHist, scale=options.histogram)
            idStats[i].add(e, key(r))
        return e

    df['e'] = df.apply(euclidian_error, axis=1)

    idStatList = list(idStats.values())
    idStatList.sort(key=lambda x: x.count())

    for x in idStatList:
        print(x)
    print(s)
    # print(dfOuter)

    if options.output:
        with open(options.output, "w") as outf:
            outf.write('<fcd-diff>\n')
            lastTime = None
            writtenTime = None
            for index, row in df.iterrows():
                time = row['t']
                if time != lastTime:
                    writtenTime = humanReadableTime(time) if options.hrTime else time
                    if lastTime is not None:
                        outf.write('   </timestep>\n')
                    outf.write('   <timestep time="%s">\n' % writtenTime)
                outf.write('       <vehicle %s/>\n' % ' '.join(['%s="%s"' % (k, v) for k, v in row.items()]))
                lastTime = time
            outf.write('   </timestep>\n')
            outf.write('</fcd-diff>\n')


if __name__ == "__main__":
    main(get_options())
