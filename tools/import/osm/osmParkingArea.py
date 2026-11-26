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

# @file    osmParkingArea.py
# @author  Mark Colley
# @date    2026-11-22

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import warnings

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Import parkingArea and polygon spaces from OSM")
    ap.add_argument("-s", "--osm-file", dest="osm_file", category="input", required=True,
                    type=ap.file, help="Input OSM XML file")
    ap.add_argument("-n", "--net-file", dest="net_file", category="input", required=True,
                    type=ap.net_file, help="Input SUMO network file")
    ap.add_argument("-o", "--output", category="output", required=True,
                    type=ap.additional_file, help="Output SUMO additional XML file")
    ap.add_argument("-r", "--radius", type=float, default=50.0, help="Search radius in meters for passenger lanes")
    ap.add_argument("-l", "--length", type=float, default=10.0, help="Stopping lane length for parking entrance")
    ap.add_argument("--vclass", default="passenger", help="Allowed vehicle class for access lane")
    return ap.parse_args(args=args)


def _match_lane(net, xy, options):
    cands = net.getNeighboringLanes(xy[0], xy[1], r=options.radius, includeJunctions=False)
    for lane, _ in sorted(cands, key=lambda c: c[1]):
        if lane.allows(options.vclass) and lane.getLength() >= 1.0:
            pos = sumolib.geomhelper.polygonOffsetWithMinimumDistanceToPoint(xy, lane.getShape())
            span = min(lane.getLength(), options.length)
            end_pos = min(lane.getLength(), max(span, pos + span * 0.5))
            return lane, max(0.0, end_pos - span), end_pos
    return None, 0.0, 0.0


def main(options):
    warnings.filterwarnings("ignore", message="Module 'rtree' not available.*")
    net = sumolib.net.readNet(options.net_file)
    nodes = {}
    node_parkings = []
    for n in sumolib.xml.parse(options.osm_file, "node"):
        xy = net.convertLonLat2XY(float(n.lon), float(n.lat))
        nodes[n.id] = xy
        tags = {t.k: t.v for t in (n.tag or [])}
        if tags.get("amenity") == "parking":
            node_parkings.append((n.id, xy, tags))
    with sumolib.openz(options.output, "w") as outf:
        sumolib.writeXMLHeader(outf, root="additional", options=options)
        for nid, xy, tags in node_parkings:
            lane, s_pos, e_pos = _match_lane(net, xy, options)
            if lane is not None:
                cap = int(tags["capacity"]) if tags.get("capacity", "").isdigit() else 5
                name = (' name=%s' % sumolib.xml.quoteattr(tags["name"])) if "name" in tags else ""
                outf.write('    <parkingArea id="parking_%s"%s lane="%s" startPos="%.2f" endPos="%.2f" '
                           'roadSideCapacity="%d"/>\n' % (nid, name, lane.getID(), s_pos, e_pos, cap))
        for w in sumolib.xml.parse(options.osm_file, "way"):
            tags = {t.k: t.v for t in (w.tag or [])}
            pts = [nodes[nd.ref] for nd in (w.nd or []) if nd.ref in nodes]
            if tags.get("amenity") != "parking" or len(pts) < 3:
                continue
            cx = sum(p[0] for p in pts) / len(pts)
            cy = sum(p[1] for p in pts) / len(pts)
            lane, s_pos, e_pos = _match_lane(net, (cx, cy), options)
            if lane is None:
                continue
            cap = int(tags["capacity"]) if tags.get("capacity", "").isdigit() else 4
            name = (' name=%s' % sumolib.xml.quoteattr(tags["name"])) if "name" in tags else ""
            shape_str = " ".join("%.2f,%.2f" % p for p in pts)
            outf.write('    <poly id="parking_poly_%s" type="parking" color="180,180,220" fill="true" '
                       'shape="%s"/>\n' % (w.id, shape_str))
            outf.write('    <parkingArea id="parking_%s"%s lane="%s" startPos="%.2f" endPos="%.2f">\n' %
                       (w.id, name, lane.getID(), s_pos, e_pos))
            for idx in range(cap):
                frac = (idx + 0.5) / max(1, cap)
                sx = pts[0][0] + frac * (cx - pts[0][0])
                sy = pts[0][1] + frac * (cy - pts[0][1])
                outf.write('        <space x="%.2f" y="%.2f"/>\n' % (sx, sy))
            outf.write("    </parkingArea>\n")
        outf.write("</additional>\n")


if __name__ == "__main__":
    main(get_options())
