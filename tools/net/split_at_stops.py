#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    split_at_stops.py
# @author  Michael Behrisch
# @date    2023-02-07

"""
This script uses an input with stop location to emit an edgediff file with splits
at the begin and end of each stop. Optionally it can write the modified network directly
and write an adapted stop and route file.
"""

from __future__ import print_function
import os
import sys
import collections
import subprocess
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    argParser = sumolib.options.ArgumentParser()
    argParser.add_argument("stopfile", nargs="*", help="stop files to process")
    argParser.add_argument("-n", "--network", help="validate positions against this network")
    argParser.add_argument("-r", "--routes", help="route file to adapt")
    argParser.add_argument("--split-output", default="splits.edg.xml", help="split file to generate")
    argParser.add_argument("-o", "--output", default="net.net.xml", help="net file to generate")
    argParser.add_argument("--stop-output", default="stops.add.xml", help="stop file to generate")
    argParser.add_argument("--route-output", default="routes.rou.xml", help="route file to generate")
    argParser.add_argument("--stop-type", default="trainStop", help="which stop types to use")
    return argParser.parse_args(args)


def main(options):
    locs = collections.defaultdict(list)
    stops = {}
    replace_edges = {}
    types = set(options.stop_type.split(","))
    for f in options.stopfile:
        for stop in sumolib.xml.parse(f, ("busStop", "trainStop")):
            if stop.name in types:
                locs[stop.lane[:stop.lane.rfind("_")]].append(stop)
            stops[stop.id] = stop
    net = sumolib.net.readNet(options.network) if options.network else None
    with open(options.split_output, "w") as out:
        sumolib.xml.writeHeader(out, root="edges", schemaPath="edgediff_file.xsd", options=options)
        for e, sl in locs.items():
            if len(sl) > 1:
                print('    <edge id="%s">' % e, file=out)
                prev_end = .1
                for s in sorted(sl, key=lambda x: float(x.endPos)):
                    start = float(s.startPos)
                    if prev_end > .1 and start < prev_end:
                        print("Skipping overlapping stop %s." % s.id)
                        continue
                    end = float(s.endPos)
                    if start > prev_end and (not net or net.getEdge(e).getLength() > start):
                        print('        <split pos="%s"/>' % s.startPos, file=out)
                    if not net or net.getEdge(e).getLength() > end + 0.1:
                        print('        <split pos="%s"/>' % s.endPos, file=out)
                    if start > 0:
                        stops[s.id].lane = e + ".%s%s" % (int(start), s.lane[s.lane.rfind("_"):])
                        stops[s.id].startPos = "0"
                        stops[s.id].endPos = "%s" % (end - start)
                        replace_edges.setdefault(e, e)
                        replace_edges[e] += " %s.%s" % (e, int(start))
                    replace_edges.setdefault(e, e)
                    replace_edges[e] += " %s.%s" % (e, int(end))
                    prev_end = end + .1
                print('    </edge>', file=out)
        print('</edges>', file=out)
    if net:
        subprocess.call([sumolib.checkBinary("netconvert"), "-s", options.network,
                         "-e", out.name, "-o", options.output])
    with sumolib.openz(options.stop_output, "w") as stop_out:
        sumolib.xml.writeHeader(stop_out, root="additional", options=options)
        for s in stops.values():
            stop_out.write(s.toXML("    "))
        print('    </additional>', file=stop_out)
    if options.routes:
        with sumolib.openz(options.routes) as route_in, sumolib.openz(options.route_output, "w") as route_out:
            for line in route_in:
                if "<route" in line:
                    eb = line.find('edges="') + 7
                    ee = line.find('"', eb)
                    ll = [replace_edges.get(e, e) for e in line[eb:ee].split()]
                    line = line[:eb] + " ".join(ll) + line[ee:]
                route_out.write(line)


if __name__ == "__main__":
    main(get_options())
