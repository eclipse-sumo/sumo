#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkStopOrder.py
# @author  Jakob Erdmann
# @date    2020-03-19

"""
Check order of arriving and leaving vehicles at stopping places.
Report if vehicles change their order at a stop (a vehicle arrives later but leaves earlier)
Also Reports if vehicle stop times are inconsistent with itself (times jump backwards)
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict
import fnmatch

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    import sumolib  # noqa
    from sumolib.miscutils import parseTime, humanReadableTime
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    parser.add_argument("-r", "--route-files", category='input', dest="routeFiles",
                        help="Input route file")
    parser.add_argument("-H", "--human-readable-time", dest="hrTime", action="store_true", default=False,
                        help="Write time values as hour:minute:second or day:hour:minute:second rathern than seconds")
    parser.add_argument("-p", "--ignore-parking", dest="ignoreParking", action="store_true", default=False,
                        help="Do not report conflicts with parking vehicles")
    parser.add_argument("--until-from-duration", action="store_true", default=False, dest="untilFromDuration",
                        help="Use stop arrival+duration instead of 'until' to compute overtaking")
    parser.add_argument("--filter-ids", category="processing", dest="filterIDs",
                        help="only consider stops for vehicles in the given list of ids")
    parser.add_argument("--stop-table", dest="stopTable",
                        help="Print timetable information for the given list of busStops")

    options = parser.parse_args(args=args)
    if options.routeFiles:
        options.routeFiles = options.routeFiles.split(',')
    else:
        print("Argument --route-files is mandatory", file=sys.stderr)
        sys.exit()

    if options.filterIDs:
        options.filterIDs = set(options.filterIDs.split(','))

    return options


def main(options):

    tf = humanReadableTime if options.hrTime else lambda x: x

    stopTimes = defaultdict(list)
    for routefile in options.routeFiles:
        for vehicle in sumolib.xml.parse(routefile, ['vehicle', 'trip'], heterogeneous=True):
            if vehicle.stop is None:
                continue
            if options.filterIDs and vehicle.id not in options.filterIDs:
                continue
            lastUntil = None
            stops = list(vehicle.stop)
            for i, stop in enumerate(stops):
                isParking = stop.parking in ["true", "True", "1"]
                if isParking and options.ignoreParking:
                    continue
                if options.untilFromDuration:
                    if stop.arrival:
                        until = parseTime(stop.arrival) + parseTime(stop.duration)
                    else:
                        print("Cannot compute 'until' for Vehicle %s because 'arrival' is not defined" %
                              vehicle.id, file=sys.stderr)
                else:
                    until = parseTime(stop.until)
                arrival = parseTime(stop.arrival) if stop.arrival else until - parseTime(stop.duration)
                if until < arrival:
                    print("Vehicle %s has 'until' before 'arrival' (%s, %s) at stop %s" % (
                        vehicle.id, tf(arrival), tf(until), stop.busStop), file=sys.stderr)
                # comparing lastUntil with arrival makes sense logically but
                # produces unnecessary warnings when using until times to encode small delays
                #
                # if lastUntil is not None and arrival < lastUntil:
                #     print("Vehicle %s has 'arrival' (%s) before previous 'until' (%s) at stop %s" % (
                #         vehicle.id, tf(arrival), tf(lastUntil), stop.busStop), file=sys.stderr)
                if lastUntil is not None and until < lastUntil:
                    print("Vehicle %s has 'until' (%s) before previous 'until' (%s) at stop %s" % (
                        vehicle.id, tf(until), tf(lastUntil), stop.busStop), file=sys.stderr)
                lastUntil = until
                flags = ''
                if isParking:
                    flags += "p"
                if i == 0:
                    flags += "F"
                if i == len(stops) - 1:
                    flags += "L"
                if float(stop.getAttributeSecure("speed", "0")) > 0:
                    flags += "w"
                stopTimes[stop.busStop].append([arrival, until, vehicle.id,
                                                stop.getAttributeSecure("tripId", ""),
                                                stop.getAttributeSecure("started", ""),
                                                stop.getAttributeSecure("ended", ""),
                                                flags
                                                ])

    for stop, times in stopTimes.items():
        times.sort()
        for i, (a, u, v, t, s, e, f) in enumerate(times):
            for i2, (a2, u2, v2, t2, s2, e2, f2) in enumerate(times[i + 1:]):
                if u2 <= u:
                    times[i][-1] += "o"
                    times[i + 1 + i2][-1] += "O"
                    print("Vehicle %s (%s, %s) overtakes %s (%s, %s) at stop %s" % (
                        v2, tf(a2), tf(u2), v, tf(a), tf(u), stop), file=sys.stderr)

    if options.stopTable:
        times = []
        stopIDs = []
        for stopID in options.stopTable.split(','):
            if stopID in stopTimes:
                stopIDs.append(stopID)
            elif "*" in stopID:
                for candID in sorted(stopTimes.keys()):
                    if fnmatch.fnmatch(candID, stopID):
                        stopIDs.append(candID)
            else:
                print("No vehicle stops at busStop '%s' found" % stopID, file=sys.stderr)

        for stopID in stopIDs:
            times += [t + [stopID] for t in stopTimes[stopID]]

        if stopIDs:
            print("# busStop: %s" % ','.join(stopIDs))
            if len(stopIDs) == 1:
                print("arrival\tuntil\tveh\ttripId\tstarted\tended\tflags")
                for a, u, v, t, s, e, f, i in sorted(times):
                    print("%s\t%s\t%s\t%s\t%s\t%s\t%s" % (tf(a), tf(u), v, t, s, e, f))
            else:
                print("arrival\tuntil\tveh\ttripId\tstarted\tended\tflags\tbusStop")
                for a, u, v, t, s, e, f, i in sorted(times):
                    print("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s" % (tf(a), tf(u), v, t, s, e, f, i))


if __name__ == "__main__":
    main(get_options())
