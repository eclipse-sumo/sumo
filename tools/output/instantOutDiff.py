#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    instantOutDiff.py
# @author  Jakob Erdmann
# @date    2025-04-13

"""
Compare differences between instantInductionLoop output files
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import bisect
from collections import defaultdict
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import Statistics, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.net import lane2edge  # noqa


def get_options(args=None):
    argParser = ArgumentParser()
    argParser.add_argument("orig", help="the first tripinfo file")
    argParser.add_argument("new", help="the second tripinfo file")
    argParser.add_argument("output", help="the output file")
    argParser.add_argument("--event-type", dest="eType", default="leave",
                           help="Which type of detection event to compare (enter, stay, leave)")
    argParser.add_argument("--combine-lanes", action="store_true", dest="combineLanes",
                           default=False, help="do not distinguish detectors by lane id")
    argParser.add_argument("--filter-ids", dest="filterIDs",
                           help="only use detector ids with the given substring")
    options = argParser.parse_args(args=args)
    return options


def parseTimes(fname, options):
    detTimes = defaultdict(list)  # detID -> [time1, time2, ...]
    for event in sumolib.xml.parse_fast(fname, 'instantOut', ['id', 'time', 'state']):
        if event.state == options.eType:
            detID = event.id
            if options.combineLanes:
                detID = lane2edge(detID)
            detTimes[detID].append(parseTime(event.time))
    return detTimes


def write_diff(options):
    origTimes = parseTimes(options.orig, options)
    newTimes = parseTimes(options.new, options)

    countMismatch = Statistics('Count mismatch')
    absCountMismatch = Statistics('Count mismatch (absolute)')
    totalTimeMismatch = Statistics('Avg Time mismatch')

    with sumolib.openz(options.output, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "instantOutDiff", options=options, rootAttrs=None)

        for det, times in origTimes.items():
            if det in newTimes:
                timeMismatch = Statistics(det)
                times2 = newTimes[det]
                iMax = len(times2)
                for t in times:
                    i = bisect.bisect_left(times2, t)
                    delta = 1e100
                    if i > 0:
                        delta = min(delta, abs(t - times2[i - 1]))
                    if i < iMax:
                        delta = min(delta, abs(t - times2[i]))
                    timeMismatch.add(delta, t)
                    totalTimeMismatch.add(delta, "%s,%s" % (det, t))
                countMismatch.add(len(times) - len(times2), det)
                absCountMismatch.add(abs(len(times) - len(times2)), det)
                outf.write(timeMismatch.toXML())
            else:
                countMismatch.add(len(times), det)
                absCountMismatch.add(len(times), det)

        outf.write('<instantOutDiff\n>')

    print(countMismatch)
    print(absCountMismatch)
    print(totalTimeMismatch)


if __name__ == "__main__":
    options = get_options()
    write_diff(options)
