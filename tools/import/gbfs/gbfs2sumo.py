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

# @file    gbfs2sumo.py
# @author  Mark Colley
# @date    2026-11-22

from __future__ import absolute_import
from __future__ import print_function

import json
import os
import sys
import warnings

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Import GBFS bikeshare station_information JSON into SUMO")
    ap.add_argument("-i", "--station-info", dest="station_info", category="input", required=True,
                    type=ap.file, help="GBFS station_information.json file")
    ap.add_argument("-s", "--station-status", dest="station_status", category="input",
                    type=ap.file, help="Optional GBFS station_status.json file")
    ap.add_argument("-n", "--net-file", dest="net_file", category="input", required=True,
                    type=ap.net_file, help="SUMO network file")
    ap.add_argument("-o", "--output", category="output", required=True,
                    type=ap.additional_file, help="Output SUMO additional XML file")
    ap.add_argument("-r", "--radius", type=float, default=50.0, help="Search radius in meters for matching lanes")
    ap.add_argument("--vclass", default="bicycle", help="Allowed vehicle class for station lanes")
    ap.add_argument("-l", "--length", type=float, default=5.0, help="Default station stopping place length in meters")
    return ap.parse_args(args=args)


def main(options):
    warnings.filterwarnings("ignore", message="Module 'rtree' not available.*")
    net = sumolib.net.readNet(options.net_file)
    with sumolib.openz(options.station_info) as inf:
        stations = json.load(inf).get("data", {}).get("stations", [])
    status_by_id = {}
    if options.station_status:
        with sumolib.openz(options.station_status) as sf:
            for st in json.load(sf).get("data", {}).get("stations", []):
                status_by_id[str(st.get("station_id", ""))] = st
    with sumolib.openz(options.output, "w") as outf:
        sumolib.writeXMLHeader(outf, root="additional", options=options)
        for st in sorted(stations, key=lambda x: str(x.get("station_id", ""))):
            sid = str(st["station_id"])
            xy = net.convertLonLat2XY(float(st["lon"]), float(st["lat"]))
            cands = net.getNeighboringLanes(xy[0], xy[1], r=options.radius, includeJunctions=False)
            lane = next((ln for ln, _ in sorted(cands, key=lambda c: c[1])
                         if ln.allows(options.vclass) and ln.getLength() >= 1.0), None)
            if lane is None:
                continue
            pos = sumolib.geomhelper.polygonOffsetWithMinimumDistanceToPoint(xy, lane.getShape())
            span = min(lane.getLength(), options.length)
            end_pos = min(lane.getLength(), max(span, pos + span * 0.5))
            cap = int(st.get("capacity", status_by_id.get(sid, {}).get("num_docks_available", 10)))
            raw_name = st.get("name", "")
            name = raw_name[0].get("text", "") if isinstance(raw_name, list) and raw_name else str(raw_name)
            name_attr = ' name=%s' % sumolib.xml.quoteattr(name) if name else ""
            outf.write('    <parkingArea id="gbfs_%s"%s lane="%s" startPos="%.2f" endPos="%.2f" '
                       'roadSideCapacity="%d">\n' % (
                           sid, name_attr, lane.getID(), max(0.0, end_pos - span), end_pos, cap))
            if sid in status_by_id and "num_bikes_available" in status_by_id[sid]:
                bikes = int(status_by_id[sid]["num_bikes_available"])
                outf.write('        <param key="bikes_available" value="%d"/>\n' % bikes)
            outf.write("    </parkingArea>\n")
        outf.write("</additional>\n")


if __name__ == "__main__":
    main(get_options())
