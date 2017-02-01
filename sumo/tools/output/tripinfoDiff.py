#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    vehrouteDiff.py
@author  Jakob Erdmann
@date    2016-15-04
@version $Id$

Compute differences between two tripinfo-output files

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
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse
from sumolib.miscutils import uMax, Statistics


def write_diff(orig, new, out):
    vehicles_orig = dict([(v.id, v) for v in parse(orig, 'tripinfo')])
    origDurations = Statistics('original durations')
    durations = Statistics('new durations')
    durationDiffs = Statistics('duration differences')
    with open(out, 'w') as f:
        f.write("<tripDiffs>\n")
        for v in parse(new, 'tripinfo'):
            if v.id in vehicles_orig:
                vOrig = vehicles_orig[v.id]
                departDiff = float(v.depart) - float(vOrig.depart)
                arrivalDiff = float(v.arrival) - float(vOrig.arrival)
                timeLossDiff = float(v.timeLoss) - float(vOrig.timeLoss)
                durationDiff = float(v.duration) - float(vOrig.duration)
                routeLengthDiff = float(v.routeLength) - \
                    float(vOrig.routeLength)

                durations.add(float(v.duration), v.id)
                origDurations.add(float(vOrig.duration), v.id)
                durationDiffs.add(durationDiff, v.id)

                f.write('''    <vehicle id="%s" departDiff="%s" arrivalDiff="%s" timeLossDiff="%s" durationDiff="%s" routeLengthDiff="%s"/>\n''' % (
                    v.id, departDiff, arrivalDiff, timeLossDiff, durationDiff, routeLengthDiff))
                del vehicles_orig[v.id]
            else:
                f.write('    <vehicle id="%s" comment="new"/>\n' % v.id)
        for id in vehicles_orig.keys():
            f.write('    <vehicle id="%s" comment="missing"/>\n' % id)
        f.write("</tripDiffs>\n")

    print(origDurations)
    print(durations)
    print(durationDiffs)

if __name__ == "__main__":
    write_diff(*sys.argv[1:])
