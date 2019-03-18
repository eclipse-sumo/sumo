#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    analyze_teleports.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2012-11-20
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import sys
import re
from collections import defaultdict


def parse_log(logfile, edges=True, aggregate=3600):
    print("Parsing %s" % logfile)
    reFrom = re.compile("lane='([^']*)'")
    reFromMeso = re.compile("edge '([^']*)'")
    reTime = re.compile("time.(\d*)\.")
    reHRTime = re.compile("time.(\d):(\d\d):(\d\d):(\d*).")
    # counts per lane
    waitingCounts = defaultdict(lambda: 0)
    collisionCounts = defaultdict(lambda: 0)
    # counts per step
    waitingStepCounts = defaultdict(lambda: 0)
    collisionStepCounts = defaultdict(lambda: 0)
    for index, line in enumerate(open(logfile)):
        try:
            if "Warning: Teleporting vehicle" in line:
                # figure out whether its micro or meso
                match = reFrom.search(line)
                if match is None:
                    edge = reFromMeso.search(line).group(1)
                else:
                    edge = match.group(1)
                    if edges:
                        edge = edge[:-2]
                timeMatch = reTime.search(line)
                if timeMatch:
                    time = int(timeMatch.group(1))
                else:
                    timeMatch = reHRTime.search(line)
                    time = (24 * 3600 * int(timeMatch.group(1))
                            + 3600 * int(timeMatch.group(2))
                            + 60 * int(timeMatch.group(3))
                            + int(timeMatch.group(4)))
                if "collision" in line:
                    collisionCounts[edge] += 1
                    collisionStepCounts[time / aggregate] += 1
                else:
                    waitingCounts[edge] += 1
                    waitingStepCounts[time / aggregate] += 1
        except Exception:
            print(sys.exc_info())
            sys.exit("error when parsing line '%s'" % line)
        if index % 1000 == 0:
            sys.stdout.write(".")
            sys.stdout.flush()
    print()
    print("read %s lines" % index)

    return (waitingCounts, collisionCounts,
            waitingStepCounts, collisionStepCounts)


def print_counts(countDict, label, num=10):
    counts = sorted(countDict.items(), key=lambda k: -k[1])
    print(label, "worst %s edges: " % num, counts[:num])
    print(label, 'total:', sum(countDict.values()))


def main(logfile):
    waitingCounts, collisionCounts, waitingStepCounts, collisionStepCounts = parse_log(logfile)
    print_counts(waitingCounts, 'waiting')
    print_counts(collisionCounts, 'collisions')
    # generate plot
    if len(waitingStepCounts) + len(collisionStepCounts) > 0:
        min_step = min(waitingStepCounts.keys() + collisionStepCounts.keys())
        max_step = max(waitingStepCounts.keys() + collisionStepCounts.keys())
        plotfile = logfile + '.plot'
        with open(plotfile, 'w') as f:
            f.write(("# plot '%s' using 1:2 with lines title 'waiting', '%s' using 1:3 with lines title " +
                     "'collisions'\n") % (plotfile, plotfile))
            for step in range(min_step, max_step + 1):
                print(' '.join(
                    map(str, [step, waitingStepCounts[step], collisionStepCounts[step]])), file=f)
    with open(logfile + "data.xml", 'w') as f:
        print('<meandata>\n    <interval begin="0" end="100:00:00:00">', file=f)
        for item in waitingCounts.items():
            print('        <edge id="%s" waiting_teleport="%s"/>' % item, file=f)
        print("    </interval>\n</meandata>", file=f)


if __name__ == "__main__":
    main(*sys.argv[1:])
