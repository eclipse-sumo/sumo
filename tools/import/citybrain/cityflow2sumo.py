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

# @file    cityflow2sumo.py
# @author  Mark Colley
# @date    2026-11-21

from __future__ import absolute_import
from __future__ import print_function

import json
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Convert CityFlow roadnet and flow JSON files to SUMO XML")
    ap.add_argument("-n", "--net-json", dest="net_json", category="input", type=ap.file,
                    help="CityFlow roadnet JSON file")
    ap.add_argument("-f", "--flow-json", dest="flow_json", category="input", type=ap.file,
                    help="CityFlow flow JSON file")
    ap.add_argument("-p", "--plain-output-prefix", dest="prefix", category="output", default="out",
                    help="Output prefix for plain XML network files (.nod.xml, .edg.xml, .con.xml)")
    ap.add_argument("-o", "--route-output", dest="route_output", category="output", type=ap.route_file,
                    help="Output SUMO route XML file")
    options = ap.parse_args(args=args)
    if not options.net_json and not options.flow_json:
        ap.error("At least one of --net-json or --flow-json must be specified.")
    return options


def convert_net(net_json, prefix, options):
    with sumolib.openz(net_json) as f:
        data = json.load(f)

    with open(prefix + ".nod.xml", "w", encoding="utf-8") as nodf:
        sumolib.writeXMLHeader(nodf, "$Id$", "nodes", options=options)
        for inter in data.get("intersections", []):
            pt = inter.get("point", {})
            has_tls = bool(inter.get("trafficLight", {}).get("lightphases")) and not inter.get("virtual", False)
            ntype = "traffic_light" if has_tls else "priority"
            nodf.write('    <node id="%s" x="%s" y="%s" type="%s"/>\n' % (
                inter["id"], pt.get("x", 0.0), pt.get("y", 0.0), ntype
            ))
        nodf.write("</nodes>\n")

    road_num_lanes = {}
    with open(prefix + ".edg.xml", "w", encoding="utf-8") as edgf:
        sumolib.writeXMLHeader(edgf, "$Id$", "edges", options=options)
        for road in data.get("roads", []):
            lanes = road.get("lanes", [])
            num_lanes = len(lanes)
            road_num_lanes[road["id"]] = num_lanes
            pts = road.get("points", [])
            shape_attr = ""
            if len(pts) > 2:
                shape_attr = ' shape="%s"' % " ".join("%s,%s" % (p["x"], p["y"]) for p in pts)
            edgf.write('    <edge id="%s" from="%s" to="%s" numLanes="%d"%s>\n' % (
                road["id"], road["startIntersection"], road["endIntersection"], num_lanes, shape_attr
            ))
            for sumo_idx, lane in enumerate(reversed(lanes)):
                edgf.write('        <lane index="%d" width="%s" speed="%s"/>\n' % (
                    sumo_idx, lane.get("width", 3.2), lane.get("maxSpeed", 13.89)
                ))
            edgf.write("    </edge>\n")
        edgf.write("</edges>\n")

    with open(prefix + ".con.xml", "w", encoding="utf-8") as conf:
        sumolib.writeXMLHeader(conf, "$Id$", "connections", options=options)
        for inter in data.get("intersections", []):
            for rl in inter.get("roadLinks", []):
                sr = rl["startRoad"]
                er = rl["endRoad"]
                for ll in rl.get("laneLinks", []):
                    fl = road_num_lanes[sr] - 1 - int(ll["startLaneIndex"])
                    tl = road_num_lanes[er] - 1 - int(ll["endLaneIndex"])
                    conf.write('    <connection from="%s" to="%s" fromLane="%d" toLane="%d"/>\n' % (sr, er, fl, tl))
        conf.write("</connections>\n")


def convert_flows(flow_json, route_output, options):
    with sumolib.openz(flow_json) as f:
        flows = json.load(f)

    vtypes = {}
    with open(route_output, "w", encoding="utf-8") as rouf:
        sumolib.writeXMLHeader(rouf, "$Id$", "routes", options=options)
        entries = []
        for idx, fl in enumerate(flows):
            veh = fl.get("vehicle", {})
            vt_key = (
                veh.get("length", 5.0), veh.get("width", 2.0),
                veh.get("maxPosAcc", 2.0), veh.get("maxNegAcc", 4.5),
                veh.get("minGap", 2.5), veh.get("maxSpeed", 16.67),
            )
            if vt_key not in vtypes:
                vtypes[vt_key] = "vtype_%d" % len(vtypes)
                rouf.write(
                    '    <vType id="%s" length="%s" width="%s" accel="%s" decel="%s" minGap="%s" maxSpeed="%s"/>\n'
                    % ((vtypes[vt_key],) + vt_key)
                )
            entries.append((idx, vtypes[vt_key], fl))

        for idx, vt_id, fl in entries:
            edges = " ".join(fl.get("route", []))
            st = fl.get("startTime", 0)
            et = fl.get("endTime", st)
            interval = fl.get("interval", 1.0)
            if st == et or interval <= 0:
                rouf.write('    <vehicle id="flow_%d" type="%s" depart="%s">\n' % (idx, vt_id, st))
                rouf.write('        <route edges="%s"/>\n' % edges)
                rouf.write("    </vehicle>\n")
            else:
                rouf.write('    <flow id="flow_%d" type="%s" begin="%s" end="%s" period="%s">\n' % (
                    idx, vt_id, st, et, interval
                ))
                rouf.write('        <route edges="%s"/>\n' % edges)
                rouf.write("    </flow>\n")
        rouf.write("</routes>\n")


def main(options):
    if options.net_json:
        convert_net(options.net_json, options.prefix, options)
    if options.flow_json:
        out_rou = options.route_output or (options.prefix + ".rou.xml")
        convert_flows(options.flow_json, out_rou, options)


if __name__ == "__main__":
    main(get_options())
