#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    computePassengerCounts.py
# @author  Jakob Erdmann
# @date    2021-10-19


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime  # noqa


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Compute Stopping Place usage")
    parser.add_argument("-s", "--stop-output-file", dest="stopOutput",
                        help="simulation stop-output file")
    parser.add_argument("-t", "--stopping-place", dest="stoppingPlace",
                        help="stoppingPlace type (busStop, parkingArea...)", default="busStop")
    parser.add_argument("--csv", action="store_true", default=False,
                        help="write in CSV format")
    parser.add_argument("--containers", action="store_true", default=False,
                        help="count containers instead of persons")
    parser.add_argument("--only-changes", action="store_true", default=False, dest="onlyChanges",
                        help="write output only for steps where the occupancy changes")
    parser.add_argument("-b", "--begin", default=None, help="begin time (when writting all steps)")
    parser.add_argument("-e", "--end", default=None, help="end time (when writting all steps)")
    options = parser.parse_args(args=args)
    if not options.stopOutput:
        parser.print_help()
        sys.exit()

    if options.begin:
        options.begin = int(parseTime(options.begin))
    if options.end:
        options.end = int(parseTime(options.end))
    return options


def main(options):
    # declare veh counts
    pCounts = defaultdict(list)

    time = None
    for stop in sumolib.xml.parse(options.stopOutput, "stopinfo"):
        initial = stop.initialPersons
        loaded = stop.loadedPersons
        unloaded = stop.unloadedPersons
        if options.containers:
            initial = stop.initialContainers
            loaded = stop.loadedContainers
            unloaded = stop.unloadedContainers

        if stop.id not in pCounts:
            pCounts[stop.id].append((parseTime(stop.started) - 1, int(initial)))
        pCounts[stop.id].append((parseTime(stop.started), -int(unloaded)))
        pCounts[stop.id].append((parseTime(stop.ended), int(loaded)))
    # iterate over pCounts
    for veh, times in pCounts.items():
        times.sort()
        steps = []
        tPrev = None
        count = 0
        for t, change in times:
            if t != tPrev and tPrev is not None:
                steps.append((tPrev, count))
            count += change
            tPrev = t
        steps.append((tPrev, count))

        if not options.onlyChanges:
            # fill missing steps
            steps2 = []
            index = 0
            number = 0
            if options.begin is not None:
                time = options.begin
                # skip steps before begin
                while steps[index][0] < time and index < len(steps):
                    index += 1
            else:
                time = int(steps[0][0])

            abort = False
            while index < len(steps):
                # fill gaps between steps
                newTime, newNumber = steps[index]
                for t in range(time, int(newTime)):
                    steps2.append((float(t), number))
                    if options.end is not None and t == options.end:
                        abort = True
                        break
                if abort:
                    break
                steps2.append((newTime, newNumber))
                if options.end is not None and newTime == options.end:
                    break
                number = newNumber
                time = int(newTime) + 1
                index += 1

            steps = steps2

        suffix = ".csv" if options.csv else ".xml"
        with open(veh + suffix, "w") as outf:
            # write header
            if options.csv:
                # write CSV header
                outf.write("step,number\n")
                for time, number in steps:
                    outf.write("%s,%s\n" % (time, number))
            else:
                # write XML header
                outf.write("<?xml version= \"1.0\" encoding=\"UTF-8\"?>\n\n")
                # open route rag
                outf.write('<vehicle id="%s">\n' % veh)
                for time, number in steps:
                    outf.write('    <step time="%s" number="%s"/>\n' % (time, number))
                outf.write("</vehicle>\n")


if __name__ == "__main__":
    main(get_options())
