#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    generateTurnRatios.py
# @author  Yun-Pang Floetteroed
# @date    2019-04-25

"""
- calculate the turn ratios or turn movements at each node
  with a given route file

- The output can be directly used as input in jtrrouter,
  where the time interval will be set for one day
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import optparse

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    import sumolib
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-o", "--output-file", dest="outfile",
                         default="turnRatios.add.xml", help="define the output filename")
    optParser.add_option("-r", "--route-files", dest="routefiles",
                         help="define the route file seperated by comma(mandatory)")
    optParser.add_option("-p", "--probabilities", dest="prob", action="store_true", default=False,
                         help=" calculate the turning probabilities instead of traffic volumes")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args(args=args)

    if not options.routefiles:
        optParser.print_help()
        sys.exit()

    return options


def getFlows(routeFiles, verbose):
    # get flows for each edge pair
    for file in routeFiles.split(','):
        edgePairFlowsMap = {}
        if verbose:
            print("route file:%s" % file)
        for veh in sumolib.output.parse(file, 'vehicle'):
            edgesList = veh.route[0].edges.split()
            for i, e in enumerate(edgesList):
                if i < len(edgesList)-1:
                    next = edgesList[i+1]
                    if e not in edgePairFlowsMap:
                        edgePairFlowsMap[e] = {}
                    if next not in edgePairFlowsMap[e]:
                        edgePairFlowsMap[e][next] = 0

                    edgePairFlowsMap[e][next] += 1
    return edgePairFlowsMap


def main(options):
    # get traffic flows for each edge pair
    edgePairFlowsMap = getFlows(options.routefiles, options.verbose)

    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "turns")  # noqa
        outf.write('    <interval begin="0" end="86400">\n')
        for from_edge in edgePairFlowsMap:
            outf.write('        <fromEdge id="%s">\n' % from_edge)
            if options.prob:
                sum = 0.
                for to_edge in edgePairFlowsMap[from_edge]:
                    sum += edgePairFlowsMap[from_edge][to_edge]
                for to_edge in edgePairFlowsMap[from_edge]:
                    outf.write('            <toEdge id="%s" probability="%.2f">\n' %
                               (to_edge, edgePairFlowsMap[from_edge][to_edge]/sum))
            else:
                for to_edge in edgePairFlowsMap[from_edge]:
                    outf.write('            <toEdge id="%s" probability="%s">\n' %
                               (to_edge, edgePairFlowsMap[from_edge][to_edge]))
            outf.write('        </fromEdge>\n')

        outf.write('    </interval>\n')
        outf.write('</turns>\n')
    outf.close()


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
