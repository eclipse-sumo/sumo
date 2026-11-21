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

# @file    gtfs_rt2sumo.py
# @author  Mark Colley
# @date    2026-11-21

from __future__ import absolute_import
from __future__ import print_function

import json
import os
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
import sumolib  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Import actual arrival/departure stop timings from GTFS-Realtime JSON"
    )
    ap.add_argument("-r", "--route-file", dest="route_file", category="input", type=ap.route_file,
                    required=True, help="Input SUMO route/vehicle XML file")
    ap.add_argument("-u", "--trip-updates", dest="trip_updates", category="input", type=ap.file,
                    required=True, help="GTFS-Realtime TripUpdate JSON file")
    ap.add_argument("-o", "--output", category="output", type=ap.route_file,
                    required=True, help="Output SUMO route XML file with started/ended stop attributes")
    ap.add_argument("--base-timestamp", dest="base_timestamp", category="time", type=int, default=0,
                    help="Epoch timestamp corresponding to simulation time 0")
    return ap.parse_args(args=args)


def main(options):
    with sumolib.openz(options.trip_updates) as f:
        feed = json.load(f)

    by_stop = {}
    by_seq = {}
    for ent in feed.get("entity", []):
        tu = ent.get("trip_update") or ent.get("tripUpdate")
        if not tu:
            continue
        trip_info = tu.get("trip", {})
        trip_id = trip_info.get("trip_id") or trip_info.get("tripId")
        if not trip_id:
            continue
        updates = tu.get("stop_time_update") or tu.get("stopTimeUpdate", [])
        for stu in updates:
            arr = stu.get("arrival") or {}
            dep = stu.get("departure") or {}
            started = int(arr["time"]) - options.base_timestamp if "time" in arr else None
            ended = int(dep["time"]) - options.base_timestamp if "time" in dep else None
            stop_id = stu.get("stop_id") or stu.get("stopId")
            seq = stu.get("stop_sequence") if "stop_sequence" in stu else stu.get("stopSequence")
            if stop_id is not None:
                by_stop[(trip_id, str(stop_id))] = (started, ended)
            if seq is not None:
                by_seq[(trip_id, int(seq))] = (started, ended)

    root = ET.parse(options.route_file).getroot()
    for veh in root:
        if veh.tag not in ("vehicle", "trip"):
            continue
        trip_id = veh.get("id")
        for par in veh.findall("param"):
            if par.get("key") in ("gtfs.trip_id", "trip_id") and par.get("value"):
                trip_id = par.get("value")
                break
        stops = veh.findall("stop")
        for idx, stop in enumerate(stops):
            bs = stop.get("busStop", "")
            timing = by_stop.get((trip_id, bs))
            if timing is None:
                for (tid, sid), val in by_stop.items():
                    if tid == trip_id and bs.endswith("_" + sid):
                        timing = val
                        break
            if timing is None:
                timing = by_seq.get((trip_id, idx + 1))
            if timing is not None:
                started, ended = timing
                if started is not None:
                    stop.set("started", str(started))
                if ended is not None:
                    stop.set("ended", str(ended))

    with open(options.output, "w", encoding="utf-8") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", options=options)
        for child in root:
            xml_str = ET.tostring(child, encoding="unicode").strip().replace(" />", "/>")
            outf.write("    %s\n" % xml_str)
        outf.write("</routes>\n")


if __name__ == "__main__":
    main(get_options())
