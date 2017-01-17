#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    analyze_teleports.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2012-11-20
@version $Id$

Extract statistics from the warning outputs of a simulation run for plotting.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import re
from collections import defaultdict


def parse_log(logfile, edges=True, aggregate=3600):
    print("Parsing %s" % logfile)
    reFrom = re.compile("lane='([^']*)'")
    reFromMeso = re.compile("edge '([^']*)'")
    reTime = re.compile("time.(\d*)\.")
    # counts per lane
    waitingCounts = defaultdict(lambda: 0)
    collisionCounts = defaultdict(lambda: 0)
    # counts per step
    waitingStepCounts = defaultdict(lambda: 0)
    collisionStepCounts = defaultdict(lambda: 0)
    for line in open(logfile):
        try:
            if "Warning: Teleporting vehicle" in line:
                # figure out whether its micro or meso
                match = reFrom.search(line)
                if match is None:
                    match = reFromMeso.search(line)
                edge = match.group(1)
                time = reTime.search(line).group(1)
                if edges:
                    edge = edge[:-2]
                if "collision" in line:
                    collisionCounts[edge] += 1
                    collisionStepCounts[int(time) / aggregate] += 1
                else:
                    waitingCounts[edge] += 1
                    waitingStepCounts[int(time) / aggregate] += 1
        except:
            print(sys.exc_info())
            sys.exit("error when parsing line '%s'" % line)

    return (waitingCounts, collisionCounts,
            waitingStepCounts, collisionStepCounts)


def print_counts(countDict, label):
    counts = [(v, k) for k, v in countDict.items()]
    counts.sort()
    print(counts)
    print(label, 'total:', sum(countDict.values()))


def main(logfile):
    waitingCounts, collisionCounts, waitingStepCounts, collisionStepCounts = parse_log(
        logfile)
    print_counts(waitingCounts, 'waiting')
    print_counts(collisionCounts, 'collisions')
    # generate plot
    if len(waitingStepCounts) + len(collisionStepCounts) > 0:
        min_step = min(waitingStepCounts.keys() + collisionStepCounts.keys())
        max_step = max(waitingStepCounts.keys() + collisionStepCounts.keys())
        plotfile = logfile + '.plot'
        with open(plotfile, 'w') as f:
            f.write("# plot '%s' using 1:2 with lines title 'waiting', '%s' using 1:3 with lines title 'collisions'\n" % (
                plotfile, plotfile))
            for step in range(min_step, max_step + 1):
                print(' '.join(
                    map(str, [step, waitingStepCounts[step], collisionStepCounts[step]])), file=f)

if __name__ == "__main__":
    main(*sys.argv[1:])
