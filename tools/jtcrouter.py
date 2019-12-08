#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    jtcrouter.py
# @author  Jakob Erdmann
# @date    2019-11-07

"""
Generate routes like jtrrouter but bas on count parameters at connections
The counts are used to derive the turn file and flow file for calling jtrrouter
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from argparse import ArgumentParser
from collections import defaultdict
import subprocess

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    parser = ArgumentParser(description="Analyze person plans")
    parser.add_argument("-n", "--net-file", dest="net", help="Input net file")
    parser.add_argument("-o", "--output-file", dest="out", default="out.rou.xml",
                        help="Output route file")
    parser.add_argument("--turn-file", dest="turnFile", default="turns.xml",
                        help="Intermediate turn-ration-file")
    parser.add_argument("--flow-file", dest="flowFile", default="flows.xml",
                        help="Intermediate flow file")
    parser.add_argument("-b", "--begin",  default=0, help="begin time")
    parser.add_argument("-e", "--end",  default=3600, help="end time (default 3600)")
    parser.add_argument("-p", "--count-param", dest="countParam", default="count",
                        help="the connection parameter to use as count")
    options = parser.parse_args(args=args)
    if options.net is None:
        parser.print_help()
        sys.exit()
    return options


def findFringe(edge, countParam, intermediateCounts=None):
    if intermediateCounts:
        # do not backtrack past edges that define turning counts (to avoid duplicate flows)
        return None
    if edge.is_fringe(edge._incoming):
        return edge
    elif len(edge.getIncoming()) == 1:
        prev = edge.getIncoming().keys()[0]
        return findFringe(prev, countParam, getCounts(prev, countParam))
    return None


def getCounts(edge, countParam):
    counts = defaultdict(lambda: 0)
    for toEdge, cons in edge.getOutgoing().items():
        for con in cons:
            value = con.getParam(countParam)
            if value is not None:
                counts[con.getTo().getID()] += float(value)
    return counts


def main(options):
    net = sumolib.net.readNet(options.net)
    with open(options.turnFile, 'w') as tf, open(options.flowFile, 'w') as ff:
        tf.write('<turns>\n')
        ff.write('<routes>\n')
        tf.write('    <interval begin="%s" end="%s">\n' % (options.begin, options.end))
        for edge in net.getEdges():
            counts = getCounts(edge, options.countParam)
            if counts:
                tf.write('        <fromEdge id="%s">\n' % (edge.getID()))
                for toEdge, count in counts.items():
                    tf.write('            <toEdge id="%s" probability="%s"/>\n' % (toEdge, count))
                tf.write('        </fromEdge>\n')

                totalCount = int(sum(counts.values()))
                fromEdge = findFringe(edge, options.countParam)
                if fromEdge:
                    ff.write('    <flow id="%s" from="%s" begin="%s" end="%s" number="%s"/>\n' % (
                        edge.getID(), fromEdge.getID(), options.begin, options.end, totalCount))
        tf.write('    </interval>\n')
        tf.write('</turns>\n')
        ff.write('</routes>\n')

    JTRROUTER = sumolib.checkBinary('jtrrouter')
    subprocess.call([JTRROUTER,
                     '-n', options.net,
                     '--turn-ratio-files', options.turnFile,
                     '--route-files', options.flowFile,
                     '--accept-all-destinations',
                     '-o', options.out])


if __name__ == "__main__":
    main(get_options())
