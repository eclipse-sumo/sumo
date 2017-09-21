#!/usr/bin/env python
"""
@file    filterFlows.py
@author  Jakob Erdmann
@date    2017-09-06
@version $Id$

This script filters csv detector data

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
from optparse import OptionParser

optParser = OptionParser(usage="usage: %prog [options] <input_flows.csv>")
optParser.add_option(
    "-f", "--flows", type="string", help="read detector flows from FILE(s) (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", type="string", help="filtered file", metavar="FILE")
optParser.add_option("-d", "--detectors", help="read detector list from file")
optParser.add_option("-b", "--begin", type="int", default=0, help="begin interval in minutes (inclusive)")
optParser.add_option("-e", "--end", type="int", default=1440, help="end interval in minutes (exclusive)")
(options, args) = optParser.parse_args()

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
