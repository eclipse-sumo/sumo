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

# @file    flowFromDetectorOutput.py
# @author  Mark Colley
# @date    2026-04-12

"""
Compare simulation E1/E2 detector XML output against reference detector flow data
with lane-level and location-level precision.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import math
import os
import sys
from collections import defaultdict

import detector
from detector import relError

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import commonprefix, parseTime  # noqa
from sumolib.xml import parse  # noqa


class LaneMap:
    def get(self, key, default):
        if key and "_" in key:
            return key.rsplit("_", 1)[0]
        return default


class ExtendedDetectorReader(detector.DetectorReader):
    def startElement(self, name, attrs):
        if name in ("e2Detector", "laneAreaDetector", "instantInductionLoop"):
            detType = attrs["type"] if "type" in attrs else None
            lane = attrs["lane"] if "lane" in attrs else attrs["lanes"].split()[0]
            pos = float(attrs["pos"]) if "pos" in attrs else 0.0
            self.addDetector(attrs["id"], pos, self._laneMap.get(lane, self._currentEdge), detType, lane)
        else:
            detector.DetectorReader.startElement(self, name, attrs)


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(
        description="Compare E1/E2 detector XML output against reference detector flow files")
    parser.add_argument("-d", "--detector-file", dest="detfile", category="input",
                        type=parser.additional_file, required=True,
                        help="read detectors from FILE (mandatory)", metavar="FILE")
    parser.add_argument("-x", "--detector-output-file", dest="detOutFiles", category="input",
                        type=parser.file_list, required=True,
                        help="read simulation E1/E2 detector XML output from FILE(s) (mandatory)",
                        metavar="FILE")
    parser.add_argument("-f", "--detector-flow-file", dest="flowfile", category="input",
                        type=parser.file,
                        help="read detector flows to compare to from FILE", metavar="FILE")
    parser.add_argument("--count-attribute", dest="countAttr", default=None, type=str,
                        category="processing",
                        help="XML interval attribute for simulated counts "
                        "(default: auto-detect 'nVehContrib' or 'nVehEntered')")
    parser.add_argument("--flow-column", dest="flowcol", default="qPKW", type=str,
                        help="which column contains reference flows", metavar="STRING")
    parser.add_argument("-z", "--respect-zero", action="store_true", dest="respectzero",
                        default=False, help="respect detectors without data (or with permanent zero) with zero flow")
    parser.add_argument("-i", "--interval", type=parser.time,
                        default="1440", help="aggregation interval in minutes")
    parser.add_argument("--long-names", action="store_true", dest="longnames",
                        default=False, help="do not use abbreviated names for detector groups")
    parser.add_argument("--edge-names", action="store_true", dest="edgenames",
                        default=False, help="include detector group edge name in output")
    parser.add_argument("-b", "--begin", type=parser.time, default=0, help="begin time in minutes")
    parser.add_argument("--end", type=parser.time, default=None, help="end time in minutes")
    parser.add_argument("-o", "--output-file", dest="output", category="output", type=parser.file,
                        help="write output to file instead of printing it to console", metavar="FILE")
    parser.add_argument("--flow-output", dest="flowout", category="output", type=parser.file,
                        help="write simulated detector output in flowfile format to FILE", metavar="FILE")
    parser.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                        default=False, help="tell me what you are doing")
    options = parser.parse_args(args=args)
    if not options.flowfile and not options.flowout:
        parser.print_help()
        sys.exit("Error! Either --detector-flow-file or --flow-output must be given.")
    options.detOutList = [f.strip() for f in options.detOutFiles.split(",") if f.strip()]
    return options


def extract_count_and_speed(interval, count_attr):
    if count_attr is not None:
        raw_cnt = interval.getAttributeSecure(count_attr, "0")
    elif interval.hasAttribute("nVehContrib"):
        raw_cnt = interval.nVehContrib
    else:
        raw_cnt = interval.getAttributeSecure("nVehEntered", "0")
    cnt = float(raw_cnt)

    raw_spd = interval.getAttributeSecure("speed", interval.getAttributeSecure("meanSpeed", "-1"))
    spd = float(raw_spd)
    return cnt, (spd if spd >= 0 else None)


def read_detector_outputs(det_out_files, begin, end, det_reader, count_attr, flowout):
    det_flow = defaultdict(float)
    iv_group_flow = defaultdict(lambda: [0.0, 0.0, False])

    for fpath in det_out_files:
        for interval in parse(fpath, "interval", attr_conversions={"begin": parseTime, "end": parseTime}):
            if interval.end <= interval.begin:
                continue
            if interval.begin < end and interval.end > begin:
                valid_iv = interval.end - interval.begin
                if interval.begin < begin:
                    valid_iv -= begin - interval.begin
                if interval.end > end:
                    valid_iv -= interval.end - end
                scale = valid_iv / (interval.end - interval.begin)
                cnt, spd = extract_count_and_speed(interval, count_attr)
                scaled_cnt = cnt * scale
                det_flow[interval.id] += scaled_cnt
                if flowout:
                    group = det_reader.getGroup(interval.id)
                    if group is not None and group.isValid:
                        key = (interval.begin / 60.0, group.ids[0])
                        entry = iv_group_flow[key]
                        old_flow, old_spd, has_spd = entry
                        new_flow = old_flow + scaled_cnt
                        if scaled_cnt > 0 and spd is not None:
                            new_spd = ((old_spd * old_flow + spd * scaled_cnt) / new_flow) if has_spd else spd
                            iv_group_flow[key] = [new_flow, new_spd, True]
                        else:
                            iv_group_flow[key] = [new_flow, old_spd, has_spd]

    if flowout and iv_group_flow:
        with open(flowout, "a", encoding="utf-8") as f:
            for (t_min, det_id), (flw, spd, has_spd) in sorted(iv_group_flow.items()):
                spd_val = "%.2f" % spd if has_spd else "-1.00"
                f.write("%s;%s;%s;%s\n" % (det_id, t_min, flw, spd_val))

    return det_flow


def group_sim_flow(group, det_flow):
    return sum(det_flow.get(did, 0.0) for did in group.ids)


def print_flows(options, det_flow, det_reader):
    edge_col = "edge " if options.edgenames else ""
    print("# detNames %sRouteFlow DetFlow ratio" % edge_col, file=options.outfile)
    output = []
    for edge, det_data in det_reader._edge2DetData.items():
        for group in det_data:
            if group.isValid:
                group_name = commonprefix(group.ids)
                if group_name == "" or options.longnames:
                    group_name = ";".join(sorted(group.ids))
                rflow = group_sim_flow(group, det_flow)
                dflow = group.totalFlow
                output.append((group_name, edge, rflow, dflow))
    for group_name, edge, rflow, dflow in sorted(output):
        if dflow > 0 or options.respectzero:
            if options.edgenames:
                print(group_name, edge, repr(rflow), repr(dflow), repr(relError(rflow, dflow)),
                      file=options.outfile)
            else:
                print(group_name, repr(rflow), repr(dflow), repr(relError(rflow, dflow)),
                      file=options.outfile)


def calc_statistics(options, begin, det_flow, det_reader):
    r_sum = 0.0
    d_sum = 0.0
    sum_abs_dev = 0.0
    sum_sq_dev = 0.0
    sum_sq_pct = 0.0
    n = 0
    for _, det_data in det_reader._edge2DetData.items():
        for group in det_data:
            if group.isValid:
                rflow = group_sim_flow(group, det_flow)
                dflow = group.totalFlow
                if dflow > 0 or options.respectzero:
                    r_sum += rflow
                    d_sum += dflow
                    dev = float(abs(rflow - dflow))
                    sum_abs_dev += dev
                    sum_sq_dev += dev * dev
                    if dflow > 0:
                        sum_sq_pct += dev * dev / dflow / dflow
                    n += 1
    print("# interval", begin, file=options.outfile)
    print("# avgRouteFlow avgDetFlow avgDev RMSE RMSPE", file=options.outfile)
    if n == 0:
        n = -1
    print("#", " ".join(map(repr, [
        r_sum / n, d_sum / n, sum_abs_dev / n,
        math.sqrt(sum_sq_dev / n), math.sqrt(sum_sq_pct / n),
    ])), file=options.outfile)


def main(options):
    options.begin = float(options.begin)
    options.interval = float(options.interval)
    if options.end is not None:
        options.end = float(options.end)

    if options.output is None:
        options.outfile = sys.stdout
    else:
        options.outfile = open(options.output, "w", encoding="utf-8")

    if options.flowout:
        with open(options.flowout, "w", encoding="utf-8") as f:
            f.write("Detector;Time;qPKW;vPKW\n")
        if not options.flowfile:
            t_min = None
            t_max = None
            for fpath in options.detOutList:
                for interval in parse(fpath, "interval", attr_conversions={"begin": float, "end": float}):
                    if t_min is None or interval.begin / 60.0 < t_min:
                        t_min = interval.begin / 60.0
                    if t_max is None or interval.end / 60.0 > t_max:
                        t_max = interval.end / 60.0
            if t_min is not None:
                options.begin = t_min
                options.end = t_max

    det_reader = ExtendedDetectorReader(options.detfile, LaneMap())
    iv_begin_m = options.begin
    have_det_flows = True
    while ((options.end is None and have_det_flows) or
           (options.end is not None and iv_begin_m < options.end)):
        iv_end_m = iv_begin_m + options.interval
        if options.end is not None:
            iv_end_m = min(iv_end_m, options.end)
        if options.flowfile:
            if options.verbose:
                print("Reading reference flows")
            have_det_flows = det_reader.readFlows(
                options.flowfile, flow=options.flowcol,
                time="Time", timeVal=iv_begin_m, timeMax=iv_end_m)
        if options.verbose:
            print("Reading detector output")
        det_flow = read_detector_outputs(
            options.detOutList, iv_begin_m * 60.0, iv_end_m * 60.0,
            det_reader, options.countAttr, options.flowout)
        if have_det_flows and options.flowfile:
            print_flows(options, det_flow, det_reader)
            calc_statistics(options, iv_begin_m, det_flow, det_reader)
            det_reader.clearFlows()
        iv_begin_m += options.interval

    if options.output is not None:
        options.outfile.close()


if __name__ == "__main__":
    main(get_options())
