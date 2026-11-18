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

# @file    vehicles2routes.py
# @author  Mark Colley
# @date    2026-04-12

"""
Convert a SUMO route file containing <vehicle> / <flow> elements into standalone
<route> definitions, optionally deduplicating unique routes with probabilities
and/or wrapping them into a <routeDistribution>.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys
from collections import OrderedDict

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import intIfPossible, parseTime  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Turn vehicles/flows in route files into standalone route definitions")
    ap.add_argument("-r", "--route-files", dest="routefiles", category="input",
                    type=ap.file_list, required=True,
                    help="comma-separated input route file(s) containing vehicles/flows")
    ap.add_argument("-o", "--output-file", dest="outfile", category="output",
                    type=ap.route_file, required=True,
                    help="output route file with standalone routes")
    ap.add_argument("-u", "--unique", action="store_true", default=False,
                    category="processing",
                    help="deduplicate identical routes and write probabilities")
    ap.add_argument("-d", "--distribution", dest="distID", type=str,
                    category="output",
                    help="wrap output routes in a <routeDistribution id='ID'> (implies --unique)")
    ap.add_argument("--counts", action="store_true", default=False,
                    category="processing",
                    help="write raw occurrence counts instead of normalized probabilities when --unique is active")
    ap.add_argument("--keep-duplicates", dest="keepDuplicates", action="store_true",
                    default=False, category="processing",
                    help="do not deduplicate routes when --distribution is used without --unique")
    ap.add_argument("--prefix", default="r_", type=str, category="output",
                    help="ID prefix for generated standalone routes (default 'r_')")
    ap.add_argument("--keep-stops", dest="keepStops", action="store_true",
                    default=False, category="processing",
                    help="preserve child <stop> elements on generated standalone routes")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)
    options.routeFileList = [f.strip() for f in options.routefiles.split(",") if f.strip()]
    return options


def get_attr_str(elem, attr_name):
    val = getattr(elem, attr_name, None)
    if isinstance(val, str):
        return val
    return elem.getAttributeSecure("attr_" + attr_name, None)


def flow_weight(elem):
    number = get_attr_str(elem, "number")
    if number is not None:
        return max(0.0, float(number))
    b = parseTime(get_attr_str(elem, "begin") or "0")
    e = parseTime(get_attr_str(elem, "end") or str(b + 3600.0))
    dur = max(0.0, e - b)
    vph = get_attr_str(elem, "vehsPerHour")
    if vph is not None and dur > 0:
        return max(0.0, float(vph) * dur / 3600.0)
    period = get_attr_str(elem, "period")
    if period is not None and parseTime(period) > 0 and dur > 0:
        return max(0.0, dur / parseTime(period))
    return 1.0


def extract_edges_and_stops(elem, standalone_routes):
    veh_stops = list(elem.stop) if isinstance(elem.stop, list) else []
    if isinstance(elem.route, list) and elem.route:
        r = elem.route[0]
        edges = r.edges.split() if r.edges else []
        r_stops = list(r.stop) if isinstance(r.stop, list) else []
        return edges, r_stops + veh_stops
    if isinstance(elem.routeDistribution, list) and elem.routeDistribution:
        rlist = elem.routeDistribution[0].route or []
        if rlist:
            last_idx = int(elem.routeDistribution[0].last or 0)
            r = rlist[min(last_idx, len(rlist) - 1)]
            edges = r.edges.split() if r.edges else []
            r_stops = list(r.stop) if isinstance(r.stop, list) else []
            return edges, r_stops + veh_stops
    r_id = get_attr_str(elem, "route")
    if r_id and r_id in standalone_routes:
        base_edges, base_stops = standalone_routes[r_id]
        return list(base_edges), list(base_stops) + veh_stops
    return [], veh_stops


def write_route_elem(outf, indent, r_id, edges, prob_str=None, stop_xmls=None):
    prob_attr = (' probability="%s"' % prob_str) if prob_str is not None else ""
    edges_str = " ".join(edges)
    if stop_xmls:
        outf.write('%s<route id="%s" edges="%s"%s>\n' % (indent, r_id, edges_str, prob_attr))
        for s_xml in stop_xmls:
            outf.write(s_xml)
        outf.write('%s</route>\n' % indent)
    else:
        outf.write('%s<route id="%s" edges="%s"%s/>\n' % (indent, r_id, edges_str, prob_attr))


def main(options):
    standalone_routes = {}
    extracted = []

    for rfile in options.routeFileList:
        if options.verbose:
            print("Reading '%s'" % rfile)
        for root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in root.getChildList():
                if elem.name == "route":
                    if elem.id and elem.edges:
                        r_stops = list(elem.stop) if isinstance(elem.stop, list) else []
                        standalone_routes[elem.id] = (elem.edges.split(), r_stops)
                elif elem.name in ("vehicle", "flow"):
                    edges, stops = extract_edges_and_stops(elem, standalone_routes)
                    if not edges:
                        continue
                    w = flow_weight(elem) if elem.name == "flow" else 1.0
                    if w <= 0:
                        continue
                    extracted.append((elem.id, tuple(edges), stops, w))

    dedup = options.unique or (options.distID is not None and not options.keepDuplicates)
    inner_indent = "        " if options.distID else "    "
    stop_indent = inner_indent + "    "

    written = 0
    with sumolib.openz(options.outfile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", schemaPath="routes_file.xsd", options=options)
        if options.distID:
            outf.write('    <routeDistribution id="%s">\n' % options.distID)

        if dedup:
            grouped = OrderedDict()
            for _, edges_tup, stops, w in extracted:
                stop_xmls = tuple(
                    st.toXML(initialIndent=stop_indent) for st in stops
                ) if options.keepStops else ()
                key = (edges_tup, stop_xmls)
                grouped[key] = grouped.get(key, 0.0) + w

            total_w = sum(grouped.values())
            for idx, ((edges_tup, stop_xmls), w) in enumerate(grouped.items()):
                r_id = "%s%d" % (options.prefix, idx)
                if options.counts:
                    p_str = str(intIfPossible(round(w, 4)))
                else:
                    p_val = (w / total_w) if total_w > 0 else 0.0
                    p_str = "%.4f" % p_val
                write_route_elem(outf, inner_indent, r_id, edges_tup,
                                 prob_str=p_str, stop_xmls=stop_xmls)
                written += 1
        else:
            seen_ids = set()
            for veh_id, edges_tup, stops, w in extracted:
                base_id = "%s%s" % (options.prefix, veh_id)
                r_id = base_id
                suffix = 1
                while r_id in seen_ids:
                    r_id = "%s_%d" % (base_id, suffix)
                    suffix += 1
                seen_ids.add(r_id)
                stop_xmls = [
                    st.toXML(initialIndent=stop_indent) for st in stops
                ] if options.keepStops else None
                prob_s = str(intIfPossible(round(w, 4))) if options.distID else None
                write_route_elem(outf, inner_indent, r_id, edges_tup,
                                 prob_str=prob_s, stop_xmls=stop_xmls)
                written += 1

        if options.distID:
            outf.write("    </routeDistribution>\n")
        outf.write("</routes>\n")

    if options.verbose:
        print("Wrote %d standalone routes (from %d vehicles/flows) to '%s'" % (
            written, len(extracted), options.outfile))


if __name__ == "__main__":
    main(get_options())
