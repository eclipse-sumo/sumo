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
from collections import OrderedDict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse  # noqa
from sumolib.miscutils import Statistics  # noqa


def write_diff(orig, new, out):
    attrs = ["depart", "arrival", "timeLoss", "duration", "routeLength"]
    attr_conversions = dict([(a, float) for a in attrs])
    vehicles_orig = OrderedDict([(v.id, v) for v in parse(orig, 'tripinfo',
        attr_conversions=attr_conversions)])
    origDurations = Statistics('original durations')
    durations = Statistics('new durations')
    durationDiffs = Statistics('duration differences')
    with open(out, 'w') as f:
        f.write("<tripDiffs>\n")
        for v in parse(new, 'tripinfo', attr_conversions=attr_conversions):
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


if __name__ == "__main__":
    write_diff(*sys.argv[1:])
