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

# @file    edgeDepartDelay.py
# @author  Jakob Erdmann
# @date    2025-01-15

"""
Compute departDelay per edge from tripinfo-output
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse_fast  # noqa
from sumolib.miscutils import Statistics, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa


def get_options(args=None):
    argParser = ArgumentParser()
    argParser.add_argument("-t", "--tripinfo-file", dest="tripinfoFile",
                           help="tripinfo file written by the simulation")
    argParser.add_argument("-o", "--output", help="the output file")
    options = argParser.parse_args(args=args)
    if not options.tripinfoFile:
        sys.exit("Required argument --tripinfo-file is missing")
    return options


def main(options):
    edgeStats = defaultdict(lambda: Statistics())
    departStats = Statistics("departure")
    delayStats = Statistics("departDelay")
    for trip in parse_fast(options.tripinfoFile, 'tripinfo', ['id', 'depart', 'departLane', 'departDelay']):
        edge = trip.departLane.rsplit('_', 1)[0]
        delay = parseTime(trip.departDelay)
        edgeStats[edge].add(delay, trip.id)
        departStats.add(parseTime(trip.depart), trip.id)
        delayStats.add(delay, trip.id)

    edgeDelayMax = Statistics("edgeDelay-max")
    edgeDelayAvg = Statistics("edgeDelay-avg")
    edgeDelayTot = Statistics("edgeDelay-total")
    for edge, stats in edgeStats.items():
        edgeDelayMax.add(stats.max, edge)
        edgeDelayAvg.add(stats.avg(), edge)
        edgeDelayTot.add(sum(stats.values), edge)

    print(departStats)
    print(delayStats)
    print(edgeDelayMax)
    print(edgeDelayAvg)
    print(edgeDelayTot)

    if options.output:
        with open(options.output, 'w') as outf:
            sumolib.writeXMLHeader(outf, "$Id$", "data", schemaPath="datamode_file.xsd", options=options)  # noqa
            outf.write('  <interval id="edgeDepartDelay" begin="%s" end="%s">\n' % (departStats.min, departStats.max))
            for edge, stats in edgeStats.items():
                outf.write(stats.toXML(tag="edge", label='', extraAttributes={'id': edge}))
            outf.write('  </interval>\n')
            outf.write('</data>\n')


if __name__ == "__main__":
    options = get_options()
    main(options)
