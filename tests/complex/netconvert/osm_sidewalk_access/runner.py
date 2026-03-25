#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @date    2026-03-25

"""
Test netconvert OSM import for correct pedestrian access handling under
various sidewalk tag combinations. Refs https://github.com/eclipse-sumo/sumo/issues/17758

Rules under test:
  - sidewalk=separate/separated -> pedestrians must NOT be on the road
                                   (unless foot=yes explicitly overrides)
  - foot=no / foot=use_sidepath -> pedestrians excluded from road
  - foot=yes                    -> pedestrians explicitly allowed on road
  - sidewalk=both/left/right    -> separate sidewalk lane added under
                                   --osm.sidewalks; pedestrians off road lanes
  - No sidewalk tags            -> pedestrians follow typemap defaults
"""

from __future__ import print_function
import os
import subprocess
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

OSM_TEMPLATE = """\
<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6">
  <node id="1" lat="51.500" lon="0.000"/>
  <node id="2" lat="51.501" lon="0.000"/>
  <way id="101">
    <nd ref="1"/>
    <nd ref="2"/>
    <tag k="highway" v="secondary"/>
    <tag k="lanes" v="2"/>
    <tag k="maxspeed" v="50"/>
    <tag k="name" v="Test Road"/>
{tags}
  </way>
</osm>
"""

def _is_sidewalk_lane(lane):
    """True if the lane is a dedicated sidewalk (pedestrian-only lane)."""
    return lane.getPermissions().issubset({"pedestrian", "wheelchair"})


def run_case(extra_tags, use_osm_sidewalks, guess_sidewalks, typemap):
    # Build OSM XML
    tag_lines = "\n".join('    <tag k="{}" v="{}"/>'.format(k, v) for k, v in extra_tags)
    osm_text = OSM_TEMPLATE.format(tags=tag_lines)
    with open("osm.xml", "w") as fh:
        fh.write(osm_text)

    # Run netconvert
    cmd = [sumolib.checkBinary("netconvert"), "--osm-files", "osm.xml", "--output-file", "test.net.xml"]
    if typemap:
        cmd += ["--type-files", typemap]
    if use_osm_sidewalks:
        cmd.append("--osm.sidewalks")
    if guess_sidewalks:
        cmd.append("--sidewalks.guess")
    subprocess.check_call(cmd, stdout=subprocess.DEVNULL)

    # Inspect the network
    net = sumolib.net.readNet("test.net.xml")
    any_sidewalk = False
    any_ped_on_road = False
    for edge in net.getEdges():
        lanes = edge.getLanes()
        sidewalk_lanes = [l for l in lanes if _is_sidewalk_lane(l)]
        road_lanes = [l for l in lanes if not _is_sidewalk_lane(l)]
        if sidewalk_lanes:
            any_sidewalk = True
        if any(l.allows("pedestrian") for l in road_lanes):
            any_ped_on_road = True
    print("tags:", extra_tags, " ".join(["netconvert"] + cmd[5:]).replace(os.environ["SUMO_HOME"], ""))
    print("    sidewalk:", any_sidewalk, "peds_on_road:", any_ped_on_road)


if __name__ == "__main__":
    pedmap = os.path.join(os.environ["SUMO_HOME"], "data", "typemap", "osmNetconvertPedestrians.typ.xml")
    for osm in (False, True):
        for guess in (False, True):
            for typemap in (None, pedmap):
                for sidewalk in (None, "yes", "no", "both", "left", "right", "separate", "none"):
                    sidewalk_tag = [("sidewalk", sidewalk)] if sidewalk else []
                    for foot in (None, "yes", "no", "designated", "use_sidepath"):
                        foot_tag = [("foot", foot)] if foot else []
                        run_case(sidewalk_tag + foot_tag, osm, guess, typemap)
