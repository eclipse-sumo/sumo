#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2021 German Aerospace Center (DLR) and others.
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

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    import sumolib  # noqa
    from sumolib.miscutils import parseTime, humanReadableTime
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    parser.add_argument("-r", "--route-files", dest="routeFiles",
                        help="Input route file")
    parser.add_argument("-H", "--human-readable-time", dest="hrTime", action="store_true", default=False,
                        help="Write time values as hour:minute:second or day:hour:minute:second rathern than seconds")
    parser.add_argument("-p", "--ignore-parking", dest="ignoreParking", action="store_true", default=False,
                        help="Do not report conflicts with parking vehicles")
    parser.add_argument("--until-from-duration", action="store_true", default=False, dest="untilFromDuration",
                        help="Use stop arrival+duration instead of 'until' to compute overtaking")
    parser.add_argument("--stop-table", dest="stopTable",
                        help="Print timetable information for the given busStop")

    options = parser.parse_args(args=args)
    if options.routeFiles:
        options.routeFiles = options.routeFiles.split(',')
    else:
        print("Argument --route-files is mandatory", file=sys.stderr)
        sys.exit()

    return options


def main(options):

    tf = humanReadableTime if options.hrTime else lambda x: x

    stopTimes = defaultdict(list)
    for routefile in options.routeFiles:
        for vehicle in sumolib.xml.parse(routefile, ['vehicle', 'trip'], heterogeneous=True):
            if vehicle.stop is None:
                continue
            lastUntil = None
            for stop in vehicle.stop:
                if stop.parking in ["true", "True", "1"] and options.ignoreParking:
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
                stopTimes[stop.busStop].append((arrival, until, vehicle.id, stop.getAttributeSecure("tripId", "")))

    for stop, times in stopTimes.items():
        times.sort()
        for i, (a, u, v, t) in enumerate(times):
            for a2, u2, v2, t2 in times[i + 1:]:
                if u2 <= u:
                    print("Vehicle %s (%s, %s) overtakes %s (%s, %s) at stop %s" % (
                        v2, tf(a2), tf(u2), v, tf(a), tf(u), stop), file=sys.stderr)

    if options.stopTable:
        if options.stopTable in stopTimes:
            times = stopTimes[options.stopTable]
            print("# busStop: %s" % options.stopTable)
            print("arrival\tuntil\tveh\ttripId")
            for a, u, v, t in sorted(times):
                print("%s\t%s\t%s\t%s" % (tf(a), tf(u), v, t))
        else:
            print("No vehicle stops at busStop '%s' found" % options.stopTable, file=sys.stderr)


if __name__ == "__main__":
    main(get_options())
