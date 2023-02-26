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
    argParser.add_argument("stopfile", nargs="+", help="stop files to process")
    argParser.add_argument("-n", "--network", help="validate positions against this network")
    argParser.add_argument("-r", "--routes", help="route file to adapt")
    argParser.add_argument("--split-output", default="splits.edg.xml", help="split file to generate")
    argParser.add_argument("-o", "--output", default="net.net.xml", help="net file to generate")
    argParser.add_argument("--stop-output", default="stops.add.xml", help="stop file to generate")
    argParser.add_argument("--route-output", default="routes.rou.xml", help="route file to generate")
    argParser.add_argument("--stop-type", default="trainStop", help="which stop types to use")
    return argParser.parse_args(args)


def check_replace(replace_edges, e, offset):
    replace_edges.setdefault(e, e)
    new_edge = "%s.%s" % (e, int(offset))
    if " " + new_edge not in replace_edges[e]:
        replace_edges[e] += " " + new_edge
        return new_edge
    return None


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
    if not locs:
        print("No stops of type '%s' found." % options.stop_type)
        return
    net = sumolib.net.readNet(options.network) if options.network else None
    with sumolib.openz(options.split_output, "w") as out:
        sumolib.xml.writeHeader(out, root="edges", schemaPath="edgediff_file.xsd", options=options)
        for e, sl in locs.items():
            if len(sl) > 1:
                skip = set()
                sorted_stops = list(sorted(sl, key=lambda x: float(x.startPos)))
                seen = set()
                prev_end = 0.
                for s in sorted_stops:
                    if float(s.startPos) == 0.:
                        skip.add(s.id)
                    if float(s.startPos) > prev_end:
                        if len(seen) > 1:
                            print("Skipping overlapping stops %s." % ", ".join(sorted(seen)))
                            skip.update(seen)
                        seen = set()
                    seen.add(s.id)
                    prev_end = max(prev_end, float(s.endPos))
                if len(seen) > 1:
                    print("Skipping overlapping stops %s." % ", ".join(sorted(seen)))
                    skip.update(seen)
                print('    <edge id="%s">' % e, file=out)
                curr_edge = None
                for s in sorted_stops:
                    start = float(s.startPos)
                    end = float(s.endPos)
                    split = False
                    if s.id not in skip and (not net or net.getEdge(e).getLength() > start):
                        new_edge = check_replace(replace_edges, e, start)
                        if new_edge:
                            print('        <split pos="%s"/>' % start, file=out)
                            prev_split = start
                            stops[s.id].lane = new_edge + s.lane[s.lane.rfind("_"):]
                            stops[s.id].startPos = "0"
                            stops[s.id].endPos = "%.2f" % (end - start)
                            split = True
                            curr_edge = new_edge
                    if curr_edge and not split:
                        stops[s.id].lane = curr_edge + s.lane[s.lane.rfind("_"):]
                        stops[s.id].startPos = "%.2f" % (start - prev_split)
                        stops[s.id].endPos = "%.2f" % (end - prev_split)
                    if s.id not in skip and (not net or net.getEdge(e).getLength() > end + .1):
                        new_edge = check_replace(replace_edges, e, end)
                        if new_edge:
                            print('        <split pos="%s"/>' % end, file=out)
                            prev_split = end
                            curr_edge = new_edge
                print('    </edge>', file=out)
        print('</edges>', file=out)
    if net:
        subprocess.call([sumolib.checkBinary("netconvert"), "-s", options.network,
                         "-e", out.name, "-o", options.output])
    with sumolib.openz(options.stop_output, "w") as stop_out:
        sumolib.xml.writeHeader(stop_out, root="additional", options=options)
        for s in stops.values():
            stop_out.write(s.toXML("    "))
        print('</additional>', file=stop_out)
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
