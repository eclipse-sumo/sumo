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

# @file    cutScenario.py
# @author  Mark Colley
# @date    2026-11-21

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    tools_dir = os.path.join(os.environ["SUMO_HOME"], "tools")
else:
    tools_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.environ["SUMO_HOME"] = os.path.dirname(tools_dir)
sys.path.append(tools_dir)
sys.path.append(os.path.join(tools_dir, "route"))
import sumolib  # noqa
from route import cutRoutes  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Cut a SUMO scenario (.sumocfg) to a bounding box")
    ap.add_argument("-s", "--sumocfg", dest="sumocfg", category="input", type=ap.sumoconfig_file,
                    required=True, help="Input SUMO configuration file")
    ap.add_argument("-b", "--bbox", category="processing", required=True,
                    help="Bounding box xmin,ymin,xmax,ymax (or west,south,east,north with --geo)")
    ap.add_argument("--geo", category="processing", action="store_true", default=False,
                    help="Interpret bounding box as geo-coordinates")
    ap.add_argument("-o", "--output-config", dest="output_config", category="output",
                    type=ap.sumoconfig_file, default="cut.sumocfg",
                    help="Output SUMO configuration file")
    return ap.parse_args(args=args)


def filter_additionals(add_files, kept_edges, kept_lanes, out_add, options):
    with open(out_add, "w", encoding="utf-8") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for add_file in add_files:
            root = ET.parse(add_file).getroot()
            for elem in root:
                lane = elem.get("lane")
                edge = elem.get("edge")
                edges_attr = elem.get("edges")
                lanes_attr = elem.get("lanes")
                if lane is not None and lane not in kept_lanes:
                    continue
                if edge is not None and edge not in kept_edges:
                    continue
                if edges_attr is not None:
                    filtered = [e for e in edges_attr.split() if e in kept_edges]
                    if not filtered:
                        continue
                    elem.set("edges", " ".join(filtered))
                if lanes_attr is not None:
                    filtered_l = [ln for ln in lanes_attr.split() if ln in kept_lanes]
                    if not filtered_l:
                        continue
                    elem.set("lanes", " ".join(filtered_l))
                xml_str = ET.tostring(elem, encoding="unicode").strip().replace(" />", "/>")
                outf.write("    %s\n" % xml_str)
        outf.write("</additional>\n")


def run_netconvert(orig_net, bbox, geo, out_net):
    bound_opt = "--keep-edges.in-geo-boundary" if geo else "--keep-edges.in-boundary"
    subprocess.check_call([sumolib.checkBinary("netconvert"), "-s", orig_net, bound_opt, bbox, "-o", out_net])


def main(options):
    cfg_dir = os.path.dirname(options.sumocfg)
    cfg_root = ET.parse(options.sumocfg).getroot()

    def get_cfg_val(section, tag):
        el = cfg_root.find("%s/%s" % (section, tag))
        return el.get("value") if el is not None else None

    net_val = get_cfg_val("input", "net-file")
    if not net_val:
        sys.exit("No net-file found in '%s'" % options.sumocfg)
    orig_net = os.path.join(cfg_dir, net_val)
    rou_val = get_cfg_val("input", "route-files")
    orig_routes = [os.path.join(cfg_dir, r.strip()) for r in rou_val.split(",") if r.strip()] if rou_val else []
    add_val = get_cfg_val("input", "additional-files")
    orig_adds = [os.path.join(cfg_dir, a.strip()) for a in add_val.split(",") if a.strip()] if add_val else []

    out_cfg = options.output_config
    out_dir = os.path.dirname(out_cfg)
    prefix = os.path.splitext(os.path.basename(out_cfg))[0]
    out_net = os.path.join(out_dir, prefix + ".net.xml")
    run_netconvert(orig_net, options.bbox, options.geo, out_net)

    sub_net = sumolib.net.readNet(out_net)
    kept_edges = set(e.getID() for e in sub_net.getEdges())
    kept_lanes = set(ln.getID() for e in sub_net.getEdges() for ln in e.getLanes())

    out_add = None
    if orig_adds:
        out_add = os.path.join(out_dir, prefix + ".add.xml")
        filter_additionals(orig_adds, kept_edges, kept_lanes, out_add, options)

    out_rou = None
    if orig_routes:
        out_rou = os.path.join(out_dir, prefix + ".rou.xml")
        cr_args = [out_net] + orig_routes + ["--routes-output", out_rou, "--orig-net", orig_net]
        if orig_adds:
            cr_args += ["--additional-input", ",".join(orig_adds)]
        cutRoutes.main(cutRoutes.get_options(cr_args))

    begin_val = get_cfg_val("time", "begin")
    end_val = get_cfg_val("time", "end")
    with open(out_cfg, "w", encoding="utf-8") as cfgf:
        sumolib.writeXMLHeader(cfgf, "$Id$", "sumoConfiguration", schemaPath="sumoConfiguration.xsd", options=options)
        cfgf.write("    <input>\n")
        cfgf.write('        <net-file value="%s"/>\n' % os.path.basename(out_net))
        if out_rou:
            cfgf.write('        <route-files value="%s"/>\n' % os.path.basename(out_rou))
        if out_add:
            cfgf.write('        <additional-files value="%s"/>\n' % os.path.basename(out_add))
        cfgf.write("    </input>\n")
        if begin_val is not None or end_val is not None:
            cfgf.write("    <time>\n")
            if begin_val is not None:
                cfgf.write('        <begin value="%s"/>\n' % begin_val)
            if end_val is not None:
                cfgf.write('        <end value="%s"/>\n' % end_val)
            cfgf.write("    </time>\n")
        cfgf.write("</sumoConfiguration>\n")


if __name__ == "__main__":
    main(get_options())
