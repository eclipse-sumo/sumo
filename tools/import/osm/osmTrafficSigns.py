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

# @file    osmTrafficSigns.py
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
    ap = sumolib.options.ArgumentParser(description="Import OSM traffic_sign elements as SUMO POIs")
    ap.add_argument("-s", "--osm-file", dest="osm_file", category="input", required=True,
                    type=ap.file, help="Input OSM XML file")
    ap.add_argument("-n", "--net-file", dest="net_file", category="input", required=True,
                    type=ap.net_file, help="Input SUMO network file")
    ap.add_argument("-o", "--output", category="output", required=True,
                    type=ap.additional_file, help="Output SUMO additional XML file with POIs")
    ap.add_argument("-r", "--radius", type=float, default=20.0,
                    help="Search radius in meters to attach POIs to nearest lane (0 for x,y POIs)")
    ap.add_argument("--vclass", default="passenger", help="Vehicle class filter when matching lanes")
    return ap.parse_args(args=args)


def main(options):
    warnings.filterwarnings("ignore", message="Module 'rtree' not available.*")
    net = sumolib.net.readNet(options.net_file)
    with sumolib.openz(options.output, "w") as outf:
        sumolib.writeXMLHeader(outf, root="additional", options=options)
        for n in sumolib.xml.parse(options.osm_file, "node"):
            tags = {t.k: t.v for t in (n.tag or [])}
            sign_key = next((k for k in sorted(tags) if k == "traffic_sign" or k.startswith("traffic_sign:")), None)
            if sign_key is None:
                continue
            sign_val = tags[sign_key]
            xy = net.convertLonLat2XY(float(n.lon), float(n.lat))
            loc_attr = 'x="%.2f" y="%.2f"' % (xy[0], xy[1])
            if options.radius > 0:
                cands = net.getNeighboringLanes(xy[0], xy[1], r=options.radius, includeJunctions=False)
                lane = next((ln for ln, _ in sorted(cands, key=lambda c: c[1]) if ln.allows(options.vclass)), None)
                if lane is not None:
                    pos, dist = sumolib.geomhelper.polygonOffsetAndDistanceToPoint(xy, lane.getShape())
                    loc_attr = 'lane="%s" pos="%.2f" posLat="%.2f"' % (lane.getID(), pos, dist)
            outf.write('    <poi id="sign_%s" type=%s color="255,0,0" layer="10" %s>\n' %
                       (n.id, sumolib.xml.quoteattr("traffic_sign:" + sign_val), loc_attr))
            for k in sorted(tags):
                if k == "traffic_sign" or k.startswith("traffic_sign:") or k == "maxspeed":
                    outf.write('        <param key=%s value=%s/>\n' %
                               (sumolib.xml.quoteattr(k), sumolib.xml.quoteattr(tags[k])))
            outf.write("    </poi>\n")
        outf.write("</additional>\n")


if __name__ == "__main__":
    main(get_options())
