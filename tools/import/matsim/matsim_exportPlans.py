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

# @file    matsim_exportPlans.py
# @author  Mark Colley
# @date    2026-04-12

"""
Export SUMO routes, trips, flows, and person plans into a MATSim population plan XML file.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys
from xml.sax import saxutils

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import humanReadableTime, parseTime  # noqa


PT_VCLASSES = {"bus", "tram", "rail", "rail_urban", "rail_electric", "rail_fast", "subway", "ship", "cable_car"}


def get_options(args=None):
    op = sumolib.options.ArgumentParser(
        description="Convert SUMO route or trip files into a MATSim plan file")
    op.add_argument("-r", "--route-files", dest="route_files", category="input", required=True,
                    type=op.route_file_list, help="SUMO route or trip files (comma-separated)")
    op.add_argument("-o", "--output-file", dest="output_file", category="output", required=True,
                    type=op.data_file, help="MATSim output plan file")
    op.add_argument("-n", "--net-file", dest="net_file", category="input", type=op.net_file,
                    help="SUMO net file (optional, for activity coordinates and route distances)")
    op.add_argument("--default-act-type", dest="default_act_type", default="unspecified",
                    category="processing", help="default activity type when none is given")
    op.add_argument("--start-act-type", dest="start_act_type", default="home",
                    category="processing", help="activity type for initial synthesized activity")
    op.add_argument("--end-act-type", dest="end_act_type", default="work",
                    category="processing", help="activity type for final synthesized activity")
    op.add_argument("--default-dur", dest="default_dur", metavar="TIME", default="01:00:00",
                    category="time", help="default duration for intermediate activities")
    op.add_argument("--default-end", dest="default_end", metavar="TIME", default="24:00:00",
                    category="time", help="default end time for the last activity")
    op.add_argument("-v", "--verbose", action="store_true", category="processing",
                    default=False, help="tell me what you are doing")
    options = op.parse_args(args)
    options.route_file_list = [f.strip() for f in options.route_files.split(",") if f.strip()]
    return options


def fmt_time(val):
    if val is None:
        return None
    return humanReadableTime(parseTime(val))


def infer_java_class(val):
    if val in ("true", "false"):
        return "java.lang.Boolean"
    try:
        int(val)
        return "java.lang.Integer"
    except ValueError:
        pass
    try:
        float(val)
        return "java.lang.Double"
    except ValueError:
        return "java.lang.String"


def lane_to_edge(lane_id):
    if lane_id and "_" in lane_id:
        return lane_id.rsplit("_", 1)[0]
    return lane_id


def resolve_stop_edge(stop):
    if stop.edge:
        return stop.edge
    if stop.lane:
        return lane_to_edge(stop.lane)
    return None


def get_edge_coord_str(net, edge_id):
    if net is None or not edge_id or not net.hasEdge(edge_id):
        return ""
    edge = net.getEdge(edge_id)
    x, y = sumolib.geomhelper.positionAtShapeOffset(edge.getShape(), edge.getLength() / 2.0)
    return ' x="%.2f" y="%.2f"' % (x, y)


def calc_route_distance_str(net, edges):
    if net is None or not edges:
        return ""
    dist = 0.0
    for eid in edges:
        if not net.hasEdge(eid):
            return ""
        dist += net.getEdge(eid).getLength()
    return ' distance="%.2f"' % dist


def vtype_to_mode(vtype_id, vtypes):
    if not vtype_id:
        return "car"
    vclass = vtypes.get(vtype_id)
    if vclass in ("bicycle", "bike") or vtype_id in ("bicycle", "bike"):
        return "bike"
    if vclass == "pedestrian" or vtype_id == "walk":
        return "walk"
    if vclass in PT_VCLASSES or vtype_id == "pt":
        return "pt"
    if vtype_id == "ride":
        return "ride"
    return "car"


def extract_edges(elem, standalone_routes):
    if isinstance(elem.route, list) and elem.route:
        if elem.route[0].edges:
            return elem.route[0].edges.split(), True
    if isinstance(elem.routeDistribution, list) and elem.routeDistribution and elem.routeDistribution[0].route:
        rlist = elem.routeDistribution[0].route
        last_idx = int(elem.routeDistribution[0].last or 0)
        chosen = rlist[min(last_idx, len(rlist) - 1)]
        if chosen.edges:
            return chosen.edges.split(), True
    route_attr = elem.route if isinstance(elem.route, str) else elem.getAttributeSecure("attr_route", None)
    if route_attr and route_attr in standalone_routes:
        return list(standalone_routes[route_attr]), True
    from_edge = elem.attr_from
    to_edge = elem.to
    if from_edge and to_edge:
        return [from_edge, to_edge], False
    if from_edge:
        return [from_edge], False
    if to_edge:
        return [to_edge], False
    return [], False


def write_attributes(outf, params):
    if not params:
        return
    outf.write("        <attributes>\n")
    for key, val in params:
        jcls = infer_java_class(val)
        outf.write('            <attribute name=%s class="%s">%s</attribute>\n' % (
            saxutils.quoteattr(key), jcls, saxutils.escape(val)))
    outf.write("        </attributes>\n")


def write_activity(outf, act_type, link, net, start_time=None, end_time=None, max_dur=None):
    attrs = ['type=%s' % saxutils.quoteattr(act_type)]
    if link:
        attrs.append('link=%s' % saxutils.quoteattr(link))
    coord_s = get_edge_coord_str(net, link)
    time_attrs = []
    if start_time is not None:
        time_attrs.append('start_time="%s"' % start_time)
    if end_time is not None:
        time_attrs.append('end_time="%s"' % end_time)
    elif max_dur is not None:
        time_attrs.append('max_dur="%s"' % max_dur)
    extra = (" " + " ".join(time_attrs)) if time_attrs else ""
    outf.write("            <activity %s%s%s/>\n" % (" ".join(attrs), coord_s, extra))


def write_leg(outf, mode, dep_time=None, edges=None, has_full_route=False, net=None):
    dep_s = (' dep_time="%s"' % dep_time) if dep_time is not None else ""
    if has_full_route and edges:
        dist_s = calc_route_distance_str(net, edges)
        outf.write('            <leg mode=%s%s>\n' % (saxutils.quoteattr(mode), dep_s))
        outf.write('                <route type="links" start_link=%s end_link=%s%s>%s</route>\n' % (
            saxutils.quoteattr(edges[0]), saxutils.quoteattr(edges[-1]),
            dist_s, " ".join(edges)))
        outf.write("            </leg>\n")
    else:
        outf.write('            <leg mode=%s%s/>\n' % (saxutils.quoteattr(mode), dep_s))


def infer_person_start_edge(person, vehicles_by_id):
    for child in person.getChildList():
        if child.name == "stop":
            e = resolve_stop_edge(child)
            if e:
                return e
        elif child.name in ("walk", "personTrip", "ride"):
            if child.attr_from:
                return child.attr_from
            if child.edges:
                return child.edges.split()[0]
            if child.name == "ride" and child.lines:
                first_line = child.lines.split()[0]
                if first_line in vehicles_by_id and vehicles_by_id[first_line]["edges"]:
                    return vehicles_by_id[first_line]["edges"][0]
    return None


def expand_flow(flow_rec):
    elem = flow_rec["elem"]
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
        return [("%s.%d" % (elem.id, i), begin + i * step) for i in range(n)]
    if period is not None:
        dt = parseTime(period)
        res = []
        t = begin
        idx = 0
        while t < end and dt > 0:
            res.append(("%s.%d" % (elem.id, idx), t))
            t += dt
            idx += 1
        return res
    if vph is not None and float(vph) > 0:
        dt = 3600.0 / float(vph)
        res = []
        t = begin
        idx = 0
        while t < end:
            res.append(("%s.%d" % (elem.id, idx), t))
            t += dt
            idx += 1
        return res
    return [(elem.id, begin)]


def export_person(outf, person, vehicles_by_id, vtypes, net, options):
    params = [(p.key, p.value) for p in (person.param or [])]
    stages = [c for c in person.getChildList() if c.name in ("stop", "walk", "ride", "personTrip")]
    if not stages:
        return False

    outf.write('    <person id=%s>\n' % saxutils.quoteattr(person.id))
    write_attributes(outf, params)
    outf.write('        <plan selected="yes">\n')

    depart_str = fmt_time(person.getAttributeSecure("depart", "0"))
    last_item_type = None
    cur_edge = infer_person_start_edge(person, vehicles_by_id)
    last_end_time = depart_str

    for idx, stage in enumerate(stages):
        if stage.name == "stop":
            stop_edge = resolve_stop_edge(stage) or cur_edge
            act_type = stage.getAttributeSecure("actType", None) or (
                options.start_act_type if idx == 0 else options.default_act_type
            )
            until_str = fmt_time(stage.until) if stage.until is not None else None
            dur_str = fmt_time(stage.duration) if stage.duration is not None else None
            start_t = depart_str if idx == 0 and depart_str is not None else None
            write_activity(outf, act_type, stop_edge, net,
                           start_time=start_t, end_time=until_str,
                           max_dur=dur_str if until_str is None else None)
            if until_str is not None:
                last_end_time = until_str
            cur_edge = stop_edge
            last_item_type = "act"
        else:
            if last_item_type != "act":
                act_t = options.start_act_type if last_item_type is None else options.default_act_type
                end_t = depart_str if last_item_type is None else None
                max_d = None if end_t is not None else "00:00:00"
                write_activity(outf, act_t, cur_edge, net, end_time=end_t, max_dur=max_d)

            dep_t = last_end_time
            last_end_time = None
            if stage.name == "walk":
                w_edges = stage.edges.split() if stage.edges else []
                to_edge = stage.to or (w_edges[-1] if w_edges else cur_edge)
                write_leg(outf, "walk", dep_time=dep_t,
                          edges=w_edges if len(w_edges) >= 2 else None,
                          has_full_route=len(w_edges) >= 2, net=net)
                cur_edge = to_edge
            elif stage.name == "personTrip":
                modes = stage.modes or ""
                if "public" in modes:
                    mode = "pt"
                elif "bicycle" in modes:
                    mode = "bike"
                elif "car" in modes:
                    mode = "car"
                else:
                    mode = "walk"
                to_edge = stage.to or cur_edge
                write_leg(outf, mode, dep_time=dep_t)
                cur_edge = to_edge
            elif stage.name == "ride":
                lines = (stage.lines or "").split()
                veh_rec = vehicles_by_id.get(lines[0]) if lines else None
                if veh_rec is not None:
                    mode = vtype_to_mode(veh_rec["type"], vtypes)
                    r_edges = veh_rec["edges"]
                    has_full = veh_rec["has_full_route"] and len(r_edges) >= 1
                    to_edge = stage.to or (r_edges[-1] if r_edges else cur_edge)
                    write_leg(outf, mode, dep_time=dep_t, edges=r_edges,
                              has_full_route=has_full, net=net)
                else:
                    mode = "pt" if (lines and lines[0] != "ANY") else "ride"
                    to_edge = stage.to or cur_edge
                    write_leg(outf, mode, dep_time=dep_t)
                cur_edge = to_edge
            last_item_type = "leg"

    if last_item_type == "leg":
        write_activity(outf, options.end_act_type, cur_edge, net,
                       end_time=fmt_time(options.default_end))

    outf.write("        </plan>\n")
    outf.write("    </person>\n")
    return True


def export_vehicle_as_person(outf, person_id, depart_sec, rec, vtypes, net, options):
    edges = rec["edges"]
    if not edges:
        return False
    mode = vtype_to_mode(rec["type"], vtypes)
    dep_str = humanReadableTime(depart_sec)
    params = rec["params"]

    outf.write('    <person id=%s>\n' % saxutils.quoteattr(person_id))
    write_attributes(outf, params)
    outf.write('        <plan selected="yes">\n')

    start_edge = edges[0]
    end_edge = edges[-1]
    write_activity(outf, options.start_act_type, start_edge, net, end_time=dep_str)

    stops = rec["stops"]
    if stops and rec["has_full_route"]:
        cur_idx = 0
        cur_dep = dep_str
        for st in stops:
            st_edge = resolve_stop_edge(st)
            if st_edge and st_edge in edges[cur_idx:]:
                hit = cur_idx + edges[cur_idx:].index(st_edge)
                seg = edges[cur_idx:hit + 1]
                write_leg(outf, mode, dep_time=cur_dep, edges=seg, has_full_route=True, net=net)
                act_t = st.getAttributeSecure("actType", None) or options.default_act_type
                until_s = fmt_time(st.until) if st.until is not None else None
                dur_s = fmt_time(st.duration) if st.duration is not None else fmt_time(options.default_dur)
                write_activity(outf, act_t, st_edge, net, end_time=until_s,
                               max_dur=dur_s if until_s is None else None)
                cur_dep = until_s
                cur_idx = hit
        rem = edges[cur_idx:]
        if len(rem) >= 2 or cur_idx == 0:
            write_leg(outf, mode, dep_time=cur_dep, edges=rem, has_full_route=True, net=net)
            write_activity(outf, options.end_act_type, end_edge, net,
                           end_time=fmt_time(options.default_end))
    else:
        write_leg(outf, mode, dep_time=dep_str, edges=edges,
                  has_full_route=rec["has_full_route"], net=net)
        write_activity(outf, options.end_act_type, end_edge, net,
                       end_time=fmt_time(options.default_end))

    outf.write("        </plan>\n")
    outf.write("    </person>\n")
    return True


def main(options):
    net = None
    if options.net_file:
        if options.verbose:
            print("Reading net '%s'" % options.net_file)
        net = sumolib.net.readNet(options.net_file)

    vtypes = {}
    standalone_routes = {}
    vehicles_by_id = {}
    ordered_vehicles = []
    persons = []
    used_vehicle_ids = set()

    for rfile in options.route_file_list:
        if options.verbose:
            print("Reading routes from '%s'" % rfile)
        for routes_root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in routes_root.getChildList():
                if elem.name == "vType":
                    vtypes[elem.id] = elem.getAttributeSecure("vClass", None)
                elif elem.name == "route":
                    if elem.id and elem.edges:
                        standalone_routes[elem.id] = elem.edges.split()
                elif elem.name in ("vehicle", "trip", "flow"):
                    edges, has_full = extract_edges(elem, standalone_routes)
                    rec = {
                        "elem": elem,
                        "kind": elem.name,
                        "type": elem.getAttributeSecure("type", None),
                        "depart": elem.getAttributeSecure("depart", None),
                        "edges": edges,
                        "has_full_route": has_full,
                        "stops": list(elem.stop) if isinstance(elem.stop, list) else [],
                        "params": [(p.key, p.value) for p in (elem.param or [])],
                    }
                    vehicles_by_id[elem.id] = rec
                    ordered_vehicles.append(rec)
                elif elem.name == "person":
                    persons.append(elem)
                    for ride in (elem.ride or []):
                        if ride.lines:
                            for lid in ride.lines.split():
                                used_vehicle_ids.add(lid)

    exported = 0
    with open(options.output_file, "w", encoding="utf-8") as outf:
        outf.write('<?xml version="1.0" encoding="utf-8"?>\n')
        outf.write('<!DOCTYPE population SYSTEM "http://www.matsim.org/files/dtd/population_v6.dtd">\n\n')
        outf.write("<population>\n")

        for person in persons:
            if export_person(outf, person, vehicles_by_id, vtypes, net, options):
                exported += 1

        for rec in ordered_vehicles:
            elem = rec["elem"]
            if elem.id in used_vehicle_ids:
                continue
            if rec["depart"] in ("triggered", "containerTriggered"):
                continue
            if rec["kind"] == "flow":
                for pid, dep_sec in expand_flow(rec):
                    if export_vehicle_as_person(outf, pid, dep_sec, rec, vtypes, net, options):
                        exported += 1
            else:
                dep_sec = parseTime(rec["depart"] or "0")
                if export_vehicle_as_person(outf, elem.id, dep_sec, rec, vtypes, net, options):
                    exported += 1

        outf.write("</population>\n")

    if options.verbose:
        print("Exported %d MATSim person plans to '%s'" % (exported, options.output_file))


if __name__ == "__main__":
    main(get_options())
