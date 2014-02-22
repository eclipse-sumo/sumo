#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    vehrouteDiff.py
@author  Jakob Erdmann
@date    2012-11-20
@version $Id$

Compute differences between two vehroute-output files

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os,sys
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', 'sumolib'))
from output import parse
from miscutils import uMax, Statistics

def update_earliest(earliest_diffs, diff, timestamp, tag):
    if timestamp < earliest_diffs[diff][0]:
        earliest_diffs[diff] = (timestamp, tag)

def write_diff(orig, new, out, earliest_out=None):
    earliest_diffs = defaultdict(lambda : (uMax, None)) # diff -> (time, veh)
    vehicles_orig = dict([(v.id, v) for v in parse(orig, 'vehicle')])
    origDurations = Statistics('original durations')
    durations = Statistics('new durations')
    durationDiffs = Statistics('duration differences')
    with open(out, 'w') as f:
        f.write("<routeDiff>\n")
        for v in parse(new, 'vehicle'):
            if v.id in vehicles_orig:
                vOrig = vehicles_orig[v.id]
                departDiff = float(v.depart) - float(vOrig.depart)
                arrivalDiff = float(v.arrival) - float(vOrig.arrival)
                exitTimes = map(float, v.route[0].exitTimes.split())
                origExitTimes = map(float, vOrig.route[0].exitTimes.split())
                exitTimesDiff = [e - eOrig for e, eOrig in zip(exitTimes, origExitTimes)]

                durations.add(float(v.arrival) - float(v.depart), v.id)
                origDurations.add(float(vOrig.arrival) - float(vOrig.depart), v.id)
                durationDiffs.add(arrivalDiff - departDiff, v.id)

                update_earliest(earliest_diffs, departDiff, vOrig.depart, v.id + ' (depart)')
                for diff, timestamp in zip(exitTimesDiff, origExitTimes):
                    update_earliest(earliest_diffs, diff, timestamp, v.id)

                f.write('''    <vehicle id="%s" departDiff="%s" arrivalDiff="%s" exitTimesDiff="%s"/>\n''' % (
                    v.id, departDiff, arrivalDiff, ' '.join(map(str, exitTimesDiff))))
                del vehicles_orig[v.id]
            else:
                f.write('    <vehicle id="%s" comment="new"/>\n' % v.id)
        for id in vehicles_orig.keys():
                f.write('    <vehicle id="%s" comment="missing"/>\n' % id)
        f.write("</routeDiff>\n")

    if earliest_out is not None:
        with open(earliest_out, 'w') as f:
            for diff in reversed(sorted(earliest_diffs.keys())):
                f.write("%s, %s\n" % (diff, earliest_diffs[diff]))

    print origDurations
    print durations
    print durationDiffs

if __name__ == "__main__":
    write_diff(*sys.argv[1:])
