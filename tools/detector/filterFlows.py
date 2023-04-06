#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    filterFlows.py
# @author  Jakob Erdmann
# @date    2017-09-06

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
from sumolib.options import ArgumentParser  # noqa

parser = ArgumentParser(usage="usage: %prog [options]")
parser.add_argument("-f", "--flows", type=parser.file, category="input", metavar="FILE",
                    help="read detector flows from FILE(s) (mandatory)")
parser.add_argument("-o", "--output", type=parser.file, category="output", help="filtered file", metavar="FILE")
parser.add_argument("-d", "--detectors", help="list of detectors to keep (defaults to *all*)")
parser.add_argument("-b", "--begin", type=parser.time, default=0, help="begin interval in minutes (inclusive)")
parser.add_argument("-e", "--end", type=parser.time, default=1440, help="end interval in minutes (exclusive)")
options = parser.parse_args()

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
