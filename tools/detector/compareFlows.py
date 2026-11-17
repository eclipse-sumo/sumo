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

# @file    compareFlows.py
# @author  Mark Colley
# @date    2026-04-12

"""
Compare total entry and exit flow counts per time interval across edgeData and
edgeRelation (turn-count) files to diagnose inconsistent boundary count inputs.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys
from collections import defaultdict

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import intIfPossible, parseTime  # noqa
from sumolib.net.connection import Connection  # noqa
from sumolib.xml import parse  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Compare total entry and exit flows across edgeData and edgeRelation files")
    ap.add_argument("-n", "--net-file", category="input", dest="netfile", type=ap.net_file,
                    help="SUMO network file (used to auto-classify boundary entry/exit edges)")
    ap.add_argument("-d", "--edgedata-files", dest="edgeDataFiles", category="input",
                    type=ap.file_list,
                    help="comma-separated edgeData XML files")
    ap.add_argument("-t", "--turn-files", dest="turnFiles", category="input",
                    type=ap.file_list,
                    help="comma-separated edgeRelation / turn-count XML files")
    ap.add_argument("--edgedata-attribute", dest="edgeDataAttr", default="entered",
                    category="processing",
                    help="attribute to read from edge elements (fallback: 'count')")
    ap.add_argument("--turn-attribute", dest="turnAttr", default="count",
                    category="processing",
                    help="attribute to read from edgeRelation elements")
    ap.add_argument("--entry-edges", dest="entryEdges", type=str, category="processing",
                    help="comma-separated list of network entry edges")
    ap.add_argument("--exit-edges", dest="exitEdges", type=str, category="processing",
                    help="comma-separated list of network exit edges")
    ap.add_argument("--vclass", default="passenger", category="processing",
                    help="vClass filter when auto-classifying boundary edges from --net-file")
    ap.add_argument("-b", "--begin", type=ap.time, category="time",
                    help="begin time in seconds or H:M:S")
    ap.add_argument("-e", "--end", type=ap.time, category="time",
                    help="end time in seconds or H:M:S")
    ap.add_argument("-i", "--interval", type=ap.time, category="time",
                    help="aggregation interval in seconds or H:M:S (default: input file intervals)")
    ap.add_argument("-o", "--output-file", dest="output", category="output", type=ap.file,
                    help="optional XML summary output file")
    ap.add_argument("-v", "--verbose", action="store_true", dest="verbose", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)

    if not options.edgeDataFiles and not options.turnFiles:
        ap.print_help()
        sys.exit("Error! At least one of --edgedata-files or --turn-files must be given.")
    if (not options.entryEdges or not options.exitEdges) and not options.netfile:
        ap.print_help()
        sys.exit("Error! Either --net-file or both --entry-edges and --exit-edges must be given.")

    options.edgeDataList = [f.strip() for f in (options.edgeDataFiles or "").split(",") if f.strip()]
    options.turnFileList = [f.strip() for f in (options.turnFiles or "").split(",") if f.strip()]
    return options


def classify_boundary_edges(net, vclass):
    entry_edges = []
    exit_edges = []
    vc = vclass if vclass else None
    for edge in sorted(net.getEdges(), key=lambda e: e.getID()):
        if edge.getFunction() == "internal":
            continue
        if vc is not None and not edge.allows(vc):
            continue
        bidi = edge.getBidi()
        inc_non_turn = [
            inc for inc, conns in edge.getIncoming().items()
            if inc.getFunction() != "internal"
            and inc != bidi
            and (vc is None or inc.allows(vc))
            and any(c.getDirection() != Connection.LINKDIR_TURN for c in conns)
        ]
        out_non_turn = [
            out for out, conns in edge.getOutgoing().items()
            if out.getFunction() != "internal"
            and out != bidi
            and (vc is None or out.allows(vc))
            and any(c.getDirection() != Connection.LINKDIR_TURN for c in conns)
        ]
        if not inc_non_turn:
            entry_edges.append(edge.getID())
        if not out_non_turn:
            exit_edges.append(edge.getID())
    return entry_edges, exit_edges


def collect_raw_intervals(options):
    raw_ed = []
    raw_turn = []
    for fpath in options.edgeDataList:
        for iv in parse(fpath, "interval", attr_conversions={"begin": parseTime, "end": parseTime}):
            if iv.end <= iv.begin:
                continue
            ed_vals = {}
            for edge in (iv.edge or []):
                raw_v = edge.getAttributeSecure(
                    options.edgeDataAttr, edge.getAttributeSecure("count", None))
                if raw_v is not None:
                    ed_vals[edge.id] = float(raw_v)
            raw_ed.append((iv.begin, iv.end, ed_vals))

    for fpath in options.turnFileList:
        for iv in parse(fpath, "interval", attr_conversions={"begin": parseTime, "end": parseTime}):
            if iv.end <= iv.begin:
                continue
            out_vals = defaultdict(float)
            in_vals = defaultdict(float)
            out_seen = set()
            in_seen = set()
            for rel in (iv.edgeRelation or []):
                raw_v = rel.getAttributeSecure(options.turnAttr, None)
                if raw_v is not None:
                    val = float(raw_v)
                    out_vals[rel.attr_from] += val
                    in_vals[rel.to] += val
                    out_seen.add(rel.attr_from)
                    in_seen.add(rel.to)
            raw_turn.append((iv.begin, iv.end, dict(out_vals), dict(in_vals), out_seen, in_seen))

    return raw_ed, raw_turn


def build_time_windows(raw_ed, raw_turn, options):
    all_spans = [(b, e) for b, e, _ in raw_ed] + [(b, e) for b, e, _, _, _, _ in raw_turn]
    if not all_spans:
        return []
    min_b = parseTime(options.begin) if options.begin is not None else min(b for b, _ in all_spans)
    max_e = parseTime(options.end) if options.end is not None else max(e for _, e in all_spans)
    if options.interval is not None:
        step = parseTime(options.interval)
        windows = []
        t = min_b
        while t < max_e and step > 0:
            windows.append((t, min(t + step, max_e)))
            t += step
        return windows

    clipped = sorted(set(
        (max(b, min_b), min(e, max_e))
        for b, e in all_spans
        if b < max_e and e > min_b
    ))
    return clipped


def overlap_scale(iv_b, iv_e, win_b, win_e):
    if iv_b >= win_e or iv_e <= win_b:
        return 0.0
    valid = min(iv_e, win_e) - max(iv_b, win_b)
    return valid / (iv_e - iv_b)


def main(options):
    auto_entry, auto_exit = ([], [])
    if options.netfile and (not options.entryEdges or not options.exitEdges):
        net = sumolib.net.readNet(options.netfile)
        auto_entry, auto_exit = classify_boundary_edges(net, options.vclass)

    entry_edges = [
        e.strip() for e in options.entryEdges.split(",") if e.strip()
    ] if options.entryEdges else auto_entry
    exit_edges = [
        e.strip() for e in options.exitEdges.split(",") if e.strip()
    ] if options.exitEdges else auto_exit

    if options.verbose:
        print("Entry edges (%d): %s" % (len(entry_edges), " ".join(entry_edges)))
        print("Exit edges (%d): %s" % (len(exit_edges), " ".join(exit_edges)))

    raw_ed, raw_turn = collect_raw_intervals(options)
    windows = build_time_windows(raw_ed, raw_turn, options)

    print("# begin end entryFlow exitFlow diff ratio missingEntry missingExit")
    rows = []
    tot_entry = 0.0
    tot_exit = 0.0

    for win_b, win_e in windows:
        ed_acc = defaultdict(float)
        ed_seen = set()
        for iv_b, iv_e, ed_vals in raw_ed:
            sc = overlap_scale(iv_b, iv_e, win_b, win_e)
            if sc > 0.0:
                for eid, v in ed_vals.items():
                    ed_acc[eid] += v * sc
                    ed_seen.add(eid)

        turn_out_acc = defaultdict(float)
        turn_in_acc = defaultdict(float)
        turn_out_seen = set()
        turn_in_seen = set()
        for iv_b, iv_e, out_vals, in_vals, o_seen, i_seen in raw_turn:
            sc = overlap_scale(iv_b, iv_e, win_b, win_e)
            if sc > 0.0:
                for eid, v in out_vals.items():
                    turn_out_acc[eid] += v * sc
                for eid, v in in_vals.items():
                    turn_in_acc[eid] += v * sc
                turn_out_seen.update(o_seen)
                turn_in_seen.update(i_seen)

        entry_sum = 0.0
        miss_entry = 0
        for eid in entry_edges:
            if eid in ed_seen:
                entry_sum += ed_acc[eid]
            elif eid in turn_out_seen:
                entry_sum += turn_out_acc[eid]
            else:
                miss_entry += 1

        exit_sum = 0.0
        miss_exit = 0
        for eid in exit_edges:
            if eid in ed_seen:
                exit_sum += ed_acc[eid]
            elif eid in turn_in_seen:
                exit_sum += turn_in_acc[eid]
            else:
                miss_exit += 1

        diff = entry_sum - exit_sum
        ratio = (exit_sum / entry_sum) if entry_sum > 0 else (1.0 if exit_sum == 0 else float("inf"))
        tot_entry += entry_sum
        tot_exit += exit_sum
        b_s = str(intIfPossible(win_b))
        e_s = str(intIfPossible(win_e))
        print("%s %s %.2f %.2f %.2f %.4f %d %d" % (
            b_s, e_s, entry_sum, exit_sum, diff, ratio, miss_entry, miss_exit))
        rows.append((b_s, e_s, entry_sum, exit_sum, diff, ratio, miss_entry, miss_exit))

    tot_diff = tot_entry - tot_exit
    tot_ratio = (tot_exit / tot_entry) if tot_entry > 0 else (1.0 if tot_exit == 0 else float("inf"))
    print("# TOTAL %.2f %.2f %.2f %.4f" % (tot_entry, tot_exit, tot_diff, tot_ratio))

    if options.output:
        with open(options.output, "w", encoding="utf-8") as outf:
            sumolib.xml.writeHeader(outf, root="flowComparison", rootAttrs=None, options=options)
            for b_s, e_s, en_f, ex_f, df, rt, m_en, m_ex in rows:
                outf.write(
                    '    <interval begin="%s" end="%s" entryFlow="%.2f" exitFlow="%.2f" '
                    'diff="%.2f" ratio="%.4f" missingEntry="%d" missingExit="%d"/>\n' % (
                        b_s, e_s, en_f, ex_f, df, rt, m_en, m_ex)
                )
            outf.write(
                '    <total entryFlow="%.2f" exitFlow="%.2f" diff="%.2f" ratio="%.4f"/>\n' % (
                    tot_entry, tot_exit, tot_diff, tot_ratio)
            )
            outf.write("</flowComparison>\n")


if __name__ == "__main__":
    main(get_options())
