#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    filterFlows.py
# @author  Jakob Erdmann
# @date    2017-09-06
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
from optparse import OptionParser

optParser = OptionParser(usage="usage: %prog [options]")
optParser.add_option(
    "-f", "--flows", type="string", help="read detector flows from FILE(s) (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", type="string", help="filtered file", metavar="FILE")
optParser.add_option("-d", "--detectors", help="read detector list from file")
optParser.add_option("-b", "--begin", type="int", default=0, help="begin interval in minutes (inclusive)")
optParser.add_option("-e", "--end", type="int", default=1440, help="end interval in minutes (exclusive)")
(options, args) = optParser.parse_args()

if options.flows is None:
    sys.exit("Option --flow must be given")
if options.output is None:
    options.output = options.flows + ".filtered.csv"

with open(options.output, 'w') as outf:
    for line in open(options.flows):
        # assume format
        # Detector;Time;qPKW;qLKW;vPKW;vLKW
        if 'qPKW' in line:
            outf.write(line)
            continue
        data = line.split(';')
        det = data[0]
        time = int(data[1])
        if options.detectors is not None and det not in options.detectors.split(','):
            continue
        if time < options.begin:
            continue
        if time >= options.end:
            continue
        outf.write(line)
