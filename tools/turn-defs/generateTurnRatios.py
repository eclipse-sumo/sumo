#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

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
from __future__ import division

import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    import sumolib
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

from sumolib.miscutils import parseTime  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_option("-r", "--route-files", required=True, type=ap.file, dest="routefiles",
                  help="define the route file separated by comma(mandatory)")
    ap.add_option("-o", "--output-file", dest="outfile", default="turnRatios.add.xml",
                  help="define the output filename")
    ap.add_option("-p", "--probabilities", dest="prob", action="store_true", default=False,
                  help=" calculate the turning probabilities instead of traffic volumes")
    ap.add_option("--id", default="generated",
                  help="define the interval id")
    ap.add_option("-b", "--begin", default="0", help="custom begin time (seconds or H:M:S)")
    ap.add_option("-e", "--end", help="custom end time (seconds or H:M:S)")
    ap.add_option("-i", "--interval", help="custom aggregation interval (seconds or H:M:S)")
    ap.add_option("-v", "--verbose", dest="verbose", action="store_true",
                  default=False, help="tell me what you are doing")
    ap.add_option("--split-types", dest="split_types", action="store_true",
                  default=False, help="split output by vehicle types")
    options = ap.parse_args(args=args)

    if options.begin is not None:
        options.begin = parseTime(options.begin)
    if options.end is not None:
        options.end = parseTime(options.end)
    else:
        options.end = 1e20
    if options.interval is not None:
        options.interval = parseTime(options.interval)

    options.routefiles = options.routefiles.split(',')

    return options


def getFlows(options):
    # get flows for each edge pair
    minDepart = 1e20
    maxDepart = 0
    begin = 0
    interval = options.interval if options.interval is not None else 1e20
    # intervalBegin -> edge -> followerEdge -> veh_type -> count
    intervalEdgePairFlowsMap = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: defaultdict(int))))
    for file in options.routefiles:
        if options.verbose:
            print("route file:%s" % file)
        for veh, route in sumolib.output.parse_fast_nested(
                file, 'vehicle', ['id', 'type', 'depart'], 'route', ['edges'],
                optional=True):
            depart = parseTime(veh.depart)
            if depart < options.begin or depart > options.end:
                continue
            #  we could also use 'DEFAULT_VEHTYPE' here but that would clash with XML attribute naming conventions
            veh_type = 'default' if veh.type is None else veh.type
            edgesList = route.edges.split()
            minDepart = min(minDepart, depart)
            maxDepart = max(maxDepart, depart)
            edgePairFlowsMap = intervalEdgePairFlowsMap[depart - depart % interval]
            for i, e in enumerate(edgesList):
                if i < len(edgesList)-1:
                    next = edgesList[i+1]
                    if e not in edgePairFlowsMap:
                        edgePairFlowsMap[e] = {}
                    if next not in edgePairFlowsMap[e]:
                        edgePairFlowsMap[e][next] = {}
                    if veh_type not in edgePairFlowsMap[e][next]:
                        edgePairFlowsMap[e][next][veh_type] = 0

                    edgePairFlowsMap[e][next][veh_type] += 1

    if options.interval is None:
        yield intervalEdgePairFlowsMap[0], minDepart, maxDepart
    else:
        for begin in sorted(intervalEdgePairFlowsMap.keys()):
            edgePairFlowsMap = intervalEdgePairFlowsMap[begin]
            yield edgePairFlowsMap, begin, begin + interval


def main(options):
    with open(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", "datamode_file.xsd")  # noqa
        # get traffic flows for each edge pair
        for edgePairFlowsMap, minDepart, maxDepart in getFlows(options):
            outf.write('    <interval id="%s" begin="%s" end="%s">\n' % (options.id, minDepart, maxDepart))
            for from_edge in sorted(edgePairFlowsMap.keys()):
                if options.split_types:
                    if options.prob:
                        # Calculate total counts for each vehicle type across all destination edges
                        total_counts = {}
                        for to_edge, type_counts in edgePairFlowsMap[from_edge].items():
                            for veh_type, count in type_counts.items():
                                if veh_type not in total_counts:
                                    total_counts[veh_type] = 0
                                total_counts[veh_type] += count

                        # Write probabilities for each edge pair and vehicle type
                        for to_edge, type_counts in sorted(edgePairFlowsMap[from_edge].items()):
                            attrs = []
                            for veh_type, count in sorted(type_counts.items()):
                                if total_counts[veh_type] > 0:
                                    prob = count / total_counts[veh_type]
                                    attrs.append('%s_probability="%.2f"' % (veh_type, prob))
                            outf.write(' ' * 8 + '<edgeRelation from="%s" to="%s" %s/>\n' % (
                                from_edge, to_edge, ' '.join(attrs)))
                    else:
                        for to_edge, type_counts in sorted(edgePairFlowsMap[from_edge].items()):
                            attrs = []
                            for veh_type, count in sorted(type_counts.items()):
                                attrs.append('%s_count="%s"' % (veh_type, count))
                            outf.write(' ' * 8 + '<edgeRelation from="%s" to="%s" %s/>\n' % (
                                from_edge, to_edge, ' '.join(attrs)))
                else:
                    if options.prob:
                        # Calculate total counts across all vehicle types and destination edges
                        total_count = 0
                        for to_edge, type_counts in edgePairFlowsMap[from_edge].items():
                            total_count += sum(type_counts.values())

                        # Write probabilities for each edge pair (combined across all vehicle types)
                        for to_edge, type_counts in sorted(edgePairFlowsMap[from_edge].items()):
                            count = sum(type_counts.values())
                            if total_count > 0:
                                prob = count / total_count
                                outf.write(' ' * 8 + '<edgeRelation from="%s" to="%s" probability="%.2f"/>\n' % (
                                    from_edge, to_edge, prob))
                    else:
                        # Write combined counts for each edge pair (summed across all vehicle types)
                        for to_edge, type_counts in sorted(edgePairFlowsMap[from_edge].items()):
                            count = sum(type_counts.values())
                            outf.write(' ' * 8 + '<edgeRelation from="%s" to="%s" count="%s"/>\n' % (
                                from_edge, to_edge, count))
            outf.write('    </interval>\n')
        outf.write('</data>\n')
    outf.close()


if __name__ == "__main__":
    options = get_options()
    main(options)
