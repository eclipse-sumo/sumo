#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    route2routeProbe.py
# @author  Mark Colley
# @date    2026-04-12

"""
Generate local <routeDistribution> definitions per edge (and optional time interval)
from route or vehroute files, equivalent to placing a <routeProbe> on every edge.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys
from collections import defaultdict

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime  # noqa


def get_options(args=None):
    op = sumolib.options.ArgumentParser(
        description="Generate per-edge routeDistribution definitions from route files")
    op.add_argument("-r", "--route-files", dest="routefiles", category="input",
                    type=op.route_file_list, required=True,
                    help="comma-separated input route or vehroute XML file(s)")
    op.add_argument("-o", "--output-file", dest="outfile", category="output",
                    type=op.route_file, required=True,
                    help="output XML file with routeDistributions")
    op.add_argument("-p", "--period", type=op.time, category="time",
                    help="aggregation interval duration in seconds or H:M:S "
                    "(default: single interval starting at --begin)")
    op.add_argument("-b", "--begin", default="0", type=op.time, category="time",
                    help="begin time in seconds or H:M:S")
    op.add_argument("-e", "--end", type=op.time, category="time",
                    help="end time in seconds or H:M:S")
    op.add_argument("--prefix", default="", type=str, category="output",
                    help="ID prefix for generated routeDistribution elements")
    op.add_argument("--edges", type=str, category="processing",
                    help="comma-separated subset of edge IDs to probe (default: all used edges)")
    op.add_argument("--remaining-only", dest="remainingOnly", action="store_true",
                    default=False, category="processing",
                    help="truncate sampled routes to start at the probed edge")
    op.add_argument("--probabilities", action="store_true", default=False,
                    category="processing",
                    help="normalize route probabilities to sum to 1 instead of writing counts")
    op.add_argument("--vtypes", type=str, category="processing",
                    help="comma-separated vehicle types to include")
    op.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = op.parse_args(args=args)

    options.routeFileList = [f.strip() for f in options.routefiles.split(",") if f.strip()]
    options.beginSec = parseTime(options.begin)
    options.endSec = parseTime(options.end) if options.end is not None else None
    options.periodSec = parseTime(options.period) if options.period is not None else None
    options.edgeSet = set(e.strip() for e in options.edges.split(",") if e.strip()) if options.edges else None
    options.vtypeSet = set(t.strip() for t in options.vtypes.split(",") if t.strip()) if options.vtypes else None
    return options


def fmt_time_label(sec):
    if abs(sec - round(sec)) < 1e-6:
        return "%.2f" % sec
    return "%.2f" % sec


def expand_flow_departures(elem):
    begin = parseTime(elem.getAttributeSecure("begin", "0"))
    end_raw = elem.getAttributeSecure("end", None)
    end = parseTime(end_raw) if end_raw is not None else begin + 3600.0
    number = elem.getAttributeSecure("number", None)
    period = elem.getAttributeSecure("period", None)
    vph = elem.getAttributeSecure("vehsPerHour", None)

    if number is not None:
        n = int(number)
        if n <= 0:
            return []
        step = (end - begin) / n if n > 1 and end > begin else 0.0
        return [(begin + i * step, 1.0) for i in range(n)]
    if period is not None and parseTime(period) > 0:
        dt = parseTime(period)
        res = []
        t = begin
        while t < end:
            res.append((t, 1.0))
            t += dt
        return res
    if vph is not None and float(vph) > 0:
        dt = 3600.0 / float(vph)
        res = []
        t = begin
        while t < end:
            res.append((t, 1.0))
            t += dt
        return res
    return [(begin, 1.0)]


def extract_route_and_exittimes(elem, standalone_routes):
    if isinstance(elem.route, list) and elem.route:
        r = elem.route[0]
        edges = r.edges.split() if r.edges else []
        exits = [parseTime(t) for t in r.exitTimes.split()] if r.getAttributeSecure("exitTimes", None) else None
        return edges, exits
    if isinstance(elem.routeDistribution, list) and elem.routeDistribution:
        rlist = elem.routeDistribution[0].route or []
        if rlist:
            last_idx = int(elem.routeDistribution[0].last or 0)
            r = rlist[min(last_idx, len(rlist) - 1)]
            edges = r.edges.split() if r.edges else []
            exits = [parseTime(t) for t in r.exitTimes.split()] if r.getAttributeSecure("exitTimes", None) else None
            return edges, exits
    r_id = elem.route if isinstance(elem.route, str) else elem.getAttributeSecure("attr_route", None)
    if r_id and r_id in standalone_routes:
        return list(standalone_routes[r_id]), None
    return [], None


def fmt_prob(val, normalize):
    if normalize:
        return "%.4f" % val
    if abs(val - round(val)) < 1e-6:
        return "%.2f" % val
    return "%.4f" % val


def main(options):
    standalone_routes = {}
    # (iv_begin, edgeID) -> route_tuple -> count
    distributions = defaultdict(lambda: defaultdict(float))

    for rfile in options.routeFileList:
        if options.verbose:
            print("Reading '%s'" % rfile)
        for root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in root.getChildList():
                if elem.name == "route":
                    if elem.id and elem.edges:
                        standalone_routes[elem.id] = elem.edges.split()
                elif elem.name in ("vehicle", "flow"):
                    if elem.getAttributeSecure("depart", None) in ("triggered", "containerTriggered"):
                        continue
                    vtype = elem.getAttributeSecure("type", "DEFAULT_VEHTYPE")
                    if options.vtypeSet is not None and vtype not in options.vtypeSet:
                        continue
                    edges, exit_times = extract_route_and_exittimes(elem, standalone_routes)
                    if not edges:
                        continue
                    if elem.name == "flow":
                        departs = expand_flow_departures(elem)
                    else:
                        departs = [(parseTime(elem.getAttributeSecure("depart", "0")), 1.0)]

                    for dep_t, weight in departs:
                        seen_on_veh = set()
                        for idx, eid in enumerate(edges):
                            if options.edgeSet is not None and eid not in options.edgeSet:
                                continue
                            if eid in seen_on_veh:
                                continue
                            seen_on_veh.add(eid)
                            if exit_times and idx > 0 and idx - 1 < len(exit_times):
                                pass_t = exit_times[idx - 1]
                            else:
                                pass_t = dep_t
                            if pass_t < options.beginSec:
                                continue
                            if options.endSec is not None and pass_t >= options.endSec:
                                continue
                            if options.periodSec is not None and options.periodSec > 0:
                                bucket_idx = int((pass_t - options.beginSec) // options.periodSec)
                                iv_begin = options.beginSec + bucket_idx * options.periodSec
                            else:
                                iv_begin = options.beginSec
                            rec_edges = tuple(edges[idx:]) if options.remainingOnly else tuple(edges)
                            distributions[(iv_begin, eid)][rec_edges] += weight

    dist_count = 0
    with sumolib.openz(options.outfile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", schemaPath="routes_file.xsd", options=options)
        for (iv_begin, eid) in sorted(distributions.keys()):
            route_map = distributions[(iv_begin, eid)]
            total = sum(route_map.values())
            if total <= 0:
                continue
            t_lbl = fmt_time_label(iv_begin)
            dist_id = "%s%s_%s" % (options.prefix, eid, t_lbl)
            outf.write('    <routeDistribution id="%s">\n' % dist_id)
            for idx, (r_edges, cnt) in enumerate(sorted(route_map.items())):
                prob_v = (cnt / total) if options.probabilities else cnt
                r_id = "%s_%d" % (dist_id, idx)
                outf.write('        <route id="%s" edges="%s" probability="%s"/>\n' % (
                    r_id, " ".join(r_edges), fmt_prob(prob_v, options.probabilities)))
            outf.write("    </routeDistribution>\n")
            dist_count += 1
        outf.write("</routes>\n")

    if options.verbose:
        print("Wrote %d routeDistributions to '%s'" % (dist_count, options.outfile))


if __name__ == "__main__":
    main(get_options())
