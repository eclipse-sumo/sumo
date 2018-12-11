#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    tripinfoDiff.py
# @author  Jakob Erdmann
# @date    2016-15-04
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import optparse
from collections import OrderedDict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse  # noqa
from sumolib.miscutils import Statistics  # noqa

def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("--persons", action="store_true",
                         default=False, help="compute personinfo differences")
    (options, args) = optParser.parse_args(args=args)
    try:
        options.orig, options.new, options.output = args
    except:
        print("USAGE: <tripinfos1> <tripinfos2> <output>", file=sys.stderr)
        sys.exit(1)
    return options


def write_diff(options):
    attrs = ["depart", "arrival", "timeLoss", "duration", "routeLength"]
    attr_conversions = dict([(a, float) for a in attrs])
    vehicles_orig = OrderedDict([(v.id, v) for v in parse(options.orig, 'tripinfo',
        attr_conversions=attr_conversions)])
    origDurations = Statistics('original durations')
    durations = Statistics('new durations')
    durationDiffs = Statistics('duration differences')
    with open(options.output, 'w') as f:
        f.write("<tripDiffs>\n")
        for v in parse(options.new, 'tripinfo', attr_conversions=attr_conversions):
            if v.id in vehicles_orig:
                vOrig = vehicles_orig[v.id]
                diffs = [v.getAttribute(a) - vOrig.getAttribute(a) for a in attrs]
                durations.add(v.duration, v.id)
                origDurations.add(vOrig.duration, v.id)
                durationDiffs.add(v.duration - vOrig.duration, v.id)
                diffAttrs = ''.join([' %sDiff="%s"' % (a,x) for a,x in zip(attrs, diffs)])
                f.write('    <vehicle id="%s"%s/>\n' % (v.id, diffAttrs))
                del vehicles_orig[v.id]
            else:
                f.write('    <vehicle id="%s" comment="new"/>\n' % v.id)
        for id in vehicles_orig.keys():
            f.write('    <vehicle id="%s" comment="missing"/>\n' % id)
        f.write("</tripDiffs>\n")

    print(origDurations)
    print(durations)
    print(durationDiffs)

def write_persondiff(options):
    attrs = ["depart", "arrival", "timeLoss", "duration", "routeLength", "waitingTime"]
    attr_conversions = dict([(a, float) for a in attrs])
    persons_orig = OrderedDict([(p.id, p) for p in parse(options.orig, 'personinfo',
        attr_conversions=attr_conversions)])
    origDurations = Statistics('original durations')
    durations = Statistics('new durations')
    durationDiffs = Statistics('duration differences')
    statAttrs = ["duration", "walkTimeLoss", "rideWait", "walks", "accesses", "rides", "stops"]
    with open(options.output, 'w') as f:
        f.write("<tripDiffs>\n")
        for p in parse(options.new, 'personinfo', attr_conversions=attr_conversions):
            if p.id in persons_orig:
                pOrig = persons_orig[p.id]
                stats = plan_stats(p)
                statsOrig = plan_stats(pOrig)
                diffs = [a - b for a,b in zip(stats, statsOrig)]
                durations.add(stats[0], p.id)
                origDurations.add(statsOrig[0], p.id)
                durationDiffs.add(stats[0] - statsOrig[0], p.id)
                diffAttrs = ''.join([' %sDiff="%s"' % (a,x) for a,x in zip(statAttrs, diffs)])
                f.write('    <personinfo id="%s"%s/>\n' % (p.id, diffAttrs))
                del persons_orig[p.id]
            else:
                f.write('    <personinfo id="%s" comment="new"/>\n' % p.id)
        for id in persons_orig.keys():
            f.write('    <personinfo id="%s" comment="missing"/>\n' % id)
        f.write("</tripDiffs>\n")

    print(origDurations)
    print(durations)
    print(durationDiffs)

def plan_stats(pInfo):
    duration = 0
    timeLoss = 0
    rideWait = 0
    walks = 0
    accesses = 0
    rides = 0
    stops = 0
    if pInfo.walk:
        walks = len(pInfo.walk)
        for walk in pInfo.walk:
            timeLoss += walk.timeLoss
            duration += walk.duration
    if pInfo.access:
        accesses = len(pInfo.access)
        for access in pInfo.access:
            duration += access.duration
    if pInfo.ride:
        rides = len(pInfo.ride)
        for ride in pInfo.ride:
            duration += ride.duration
            rideWait += ride.waitingTime
    if pInfo.stop:
        stops = len(pInfo.stop)
        for stop in pInfo.stop:
            duration += stop.duration
    return (duration, timeLoss, rideWait, walks, accesses, rides, stops)


if __name__ == "__main__":
    options = get_options()
    if options.persons:
        write_persondiff(options)
    else:
        write_diff(options)
