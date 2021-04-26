#!/usr/bin/env python
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

# @file    scheduleStats.py
# @author  Jakob Erdmann
# @date    2021-04-20

"""
Compare arrival and departur at stops between an input schedule (route-file) and
simulation output (stop-output).
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict
import pandas as pd

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime, humanReadableTime  # noqa
from sumolib.statistics import Statistics  # noqa
from sumolib.xml import parse  # noqa

pd.options.display.width = 0 # auto-detect terminal width

STATS = {
 # selector -> description, function
 'd' : ('departDelay',  lambda r,s: s.add(r['ended'] - r['until'], key(r))),
 'a' : ('arrivalDelay', lambda r,s: s.add(r['started'] - r['arrival'], key(r))),
 's' : ('stopDelay',    lambda r,s: s.add(r['until'] - r['arrival'] - (r['ended'] - r['started']) , key(r))), #  noqua
 }


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Compare route-file stop timing with stop-output")
    parser.add_argument("-r", "--route-file", dest="routeFile",
                        help="Input route file")
    parser.add_argument("-s", "--stop-file", dest="stopFile",
                        help="Input stop-output file")
    parser.add_argument("-t", "--statistic-type", default="d", dest="sType",
                        help="Code for statistic type from %s" % STATS.keys())
    parser.add_argument("-g", "--group-by", dest="groupBy",
                        help="Code for grouping results")
    parser.add_argument("-H", "--human-readable-time", dest="hrTime", action="store_true", default=False,
                        help="Write time values as hour:minute:second or day:hour:minute:second rathern than seconds")
    parser.add_argument("-v", "--verbose", action="store_true",
                        default=False, help="tell me what you are doing")

    options = parser.parse_args(args=args)
    if options.routeFile is None or options.stopFile is None:
        parser.print_help()
        sys.exit()

    if options.sType not in STATS:
        parser.print_help()
        sys.exit()

    if options.groupBy:
        options.groupBy = options.groupBy.split(',')

    return options

ATTR_CONVERSIONS = { 
    'arrival' : parseTime,
    'until' : parseTime,
    'started' : parseTime,
    'ended' : parseTime,
    }

def getStopID(stop):
    if stop.hasAttribute("busStop"):
        return stop.busStop
    else:
        #stopinfo has no endPos, only pos
        #return "%s,%s" % (stop.lane, stop.endPos)
        return "%s" % stop.lane

def key(row):
    return "%s_%s" % (row['tripId'], row['stopID'])


def main(options):
    nan = float("nan")

    columns = [
            'vehID',
            'tripId',   # tripId of current stop or set by earlier stop
            'stopID',   # busStop id or lane,pos
            'priorStop', # busStop id or lane,pos
            'arrival',  # route-input
            'until',    # route-input   
            ]

    columns2 = columns[:3] + [
            'started',  # stop-output
            'ended',    # stop-input   
            ]

    stops = []
    tripIds = dict() # vehID -> lastTripId
    priorStops = dict() # vehID -> lastStopID
    for vehicle in parse(options.routeFile, ['vehicle', 'trip'],
            heterogeneous=True, attr_conversions=ATTR_CONVERSIONS):
        if vehicle.stop is not None:
            for stop in vehicle.stop:
                vehID = vehicle.id
                tripId = stop.getAttributeSecure("tripId", tripIds.get(vehID))
                tripIds[vehID] = tripId
                stopID = getStopID(stop)
                priorStop = priorStops.get(vehID)
                priorStops[vehID] = stopID

                stops.append((vehID, tripId, stopID, priorStop,
                    stop.getAttributeSecure("arrival", nan),
                    stop.getAttributeSecure("until", nan)))

    print("Parsed %s stops" % len(stops))

    simStops = []
    tripIds = dict() # vehID -> lastTripId
    priorStops = dict() # vehID -> lastStopID
    for stop in parse(options.stopFile, "stopinfo", heterogeneous=True,
            attr_conversions=ATTR_CONVERSIONS):
        vehID = stop.id
        tripId = stop.getAttributeSecure("tripId", tripIds.get(vehID))
        tripIds[vehID] = tripId
        stopID = getStopID(stop)
        priorStop = priorStops.get(vehID)
        priorStops[vehID] = stopID

        simStops.append((vehID, tripId, stopID, #  priorStop,
            stop.getAttributeSecure("started", nan),
            stop.getAttributeSecure("ended", nan)))

    print("Parsed %s stopinfos" % len(simStops))

    dfSchedule = pd.DataFrame.from_records(stops, columns=columns)
    dfSim = pd.DataFrame.from_records(simStops, columns=columns2)
    # merge on common columns vehID, tripId, stopID
    df = pd.merge(dfSchedule, dfSim,
            on=columns[:3],
            #how="outer",
            how="inner",
            )

    print("Found %s matches" % len(df))

    if options.verbose:
        #print(dfSchedule)
        #print(dfSim)
        print(df)


    description, fun = STATS[options.sType]
    if options.groupBy:
        numGroups = 0
        for name, group in df.groupby(options.groupBy):
            numGroups += 1;
            s = Statistics("%s:%s" % (description,name), abs=True)
            group.apply(fun, axis=1, args=(s,))
            print(s)
        print(numGroups, "groups")
    else:
        s = Statistics(description, abs=True)
        df.apply(fun, axis=1, args=(s,))
        print(s)

if __name__ == "__main__":
    main(get_options())
