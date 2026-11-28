#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2022-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateTrafficVariations.py
# @author  Jakob Erdmann
# @date    2026-11-19

"""
Generate systematic traffic demand variations across N input flows using reflected
or balanced Gray codes and optional smooth transition curves.
"""
from __future__ import absolute_import
from __future__ import print_function

import copy
import math
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import sumolib  # noqa


# Exact balanced cyclic binary Gray transition sequences (flip bit indices for 2^N states)
BALANCED_BINARY_FLIPS = {
    1: [0, 0],
    2: [0, 1, 0, 1],
    3: [0, 1, 2, 1, 0, 1, 2, 1],
    4: [0, 1, 2, 3, 2, 1, 0, 2, 0, 3, 2, 1, 3, 2, 3, 1],
    5: [
        0, 1, 2, 3, 4, 1, 2, 0, 2, 4, 0, 1, 0, 3, 0, 4,
        3, 1, 3, 0, 3, 4, 2, 4, 3, 2, 1, 4, 1, 2, 4, 3,
    ],
}


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Generate Gray-code traffic demand variations from base flows")
    ap.add_argument("-r", "--route-file", dest="routeFile", category="input", required=True, type=ap.route_file,
                    help="Input route file containing base <flow> definitions")
    ap.add_argument("-o", "--output-file", dest="out", category="output", required=True, type=ap.route_file,
                    help="Output route file with time-varied <flow> intervals")
    ap.add_argument("--levels", default="0.5,1.0",
                    help="Comma-separated demand scaling factors (default '0.5,1.0')")
    ap.add_argument("--step-duration", dest="stepDuration", default=900, type=ap.time,
                    help="Duration in seconds of each Gray-code combination (default 900)")
    ap.add_argument("--substeps", type=int, default=1,
                    help="Sub-intervals per step for smooth interpolation curves (default 1)")
    ap.add_argument("--curve", choices=("step", "linear", "sine", "sawtooth", "haar"), default="step",
                    help="Interpolation waveform between consecutive states (default 'step')")
    ap.add_argument("--balanced", action="store_true", default=False,
                    help="Use balanced Gray code to equalize transition frequencies across streams")
    ap.add_argument("-b", "--begin", default=0, type=ap.time,
                    help="Start time of the first generated interval (default 0)")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Print Gray-code state count and per-stream transition statistics")
    options = ap.parse_args(args=args)
    options.levels = [float(x) for x in options.levels.split(",") if x.strip()]
    if len(options.levels) < 2:
        sys.exit("At least two --levels must be specified.")
    if options.substeps < 1:
        sys.exit("--substeps must be >= 1.")
    return options


def reflected_kary_gray(num_streams, base):
    total = base ** num_streams
    states = []
    for idx in range(total):
        digits = [0] * num_streams
        val = idx
        for pos in range(num_streams):
            rem = val % base
            val //= base
            digits[pos] = (base - 1 - rem) if (val % 2 == 1) else rem
        states.append(tuple(digits))
    return states


def _search_balanced_binary_gray(num_streams):
    total = 1 << num_streams
    target_max = math.ceil(total / float(num_streams))
    used = {0}
    seq = [0]
    counts = [0] * num_streams

    def backtrack(cur):
        if len(seq) == total:
            # Prefer cyclic closure if within target_max
            diff = cur
            if diff and (diff & (diff - 1)) == 0:
                b = diff.bit_length() - 1
                if counts[b] + 1 <= target_max + 1:
                    return True
            return False
        order = sorted(range(num_streams), key=lambda i: (counts[i], i))
        for b in order:
            if counts[b] >= target_max:
                continue
            nxt = cur ^ (1 << b)
            if nxt not in used:
                used.add(nxt)
                seq.append(nxt)
                counts[b] += 1
                if backtrack(nxt):
                    return True
                counts[b] -= 1
                seq.pop()
                used.remove(nxt)
        return False

    if backtrack(0):
        return [tuple((s >> i) & 1 for i in range(num_streams)) for s in seq]
    return None


def generate_gray_states(num_streams, base, balanced=False):
    if num_streams == 0:
        return []
    if balanced and base == 2:
        if num_streams in BALANCED_BINARY_FLIPS:
            cur = [0] * num_streams
            states = []
            seen = set()
            for flip in BALANCED_BINARY_FLIPS[num_streams]:
                t = tuple(cur)
                states.append(t)
                seen.add(t)
                cur[flip] ^= 1
            if len(seen) == (1 << num_streams):
                return states
        if num_streams <= 5:
            found = _search_balanced_binary_gray(num_streams)
            if found is not None:
                return found
    return reflected_kary_gray(num_streams, base)


def interp_weight(curve, frac):
    if curve == "step":
        return 0.0
    if curve == "linear" or curve == "sawtooth":
        return frac
    if curve == "sine":
        return 0.5 * (1.0 - math.cos(math.pi * frac))
    if curve == "haar":
        return 0.0 if frac < 0.5 else 1.0
    return 0.0


def format_num(val):
    return "%g" % round(val, 6) if float(round(val, 6)).is_integer() else ("%.4f" % val).rstrip("0").rstrip(".")


def scale_flow_elem(base_flow, flow_id, beg, end, scale):
    if scale <= 0.0:
        return None
    elem = copy.deepcopy(base_flow)
    elem.setAttribute("id", flow_id)
    elem.setAttribute("begin", format_num(beg))
    elem.setAttribute("end", format_num(end))
    if elem.hasAttribute("vehsPerHour"):
        elem.setAttribute("vehsPerHour", format_num(float(base_flow.vehsPerHour) * scale))
    elif elem.hasAttribute("perHour"):
        elem.setAttribute("perHour", format_num(float(base_flow.perHour) * scale))
    elif elem.hasAttribute("probability"):
        elem.setAttribute("probability", format_num(min(1.0, float(base_flow.probability) * scale)))
    elif elem.hasAttribute("period"):
        elem.setAttribute("period", format_num(float(base_flow.period) / scale))
    elif elem.hasAttribute("number"):
        orig_dur = max(1.0, float(base_flow.end) - float(base_flow.begin)) if (
            base_flow.hasAttribute("begin") and base_flow.hasAttribute("end")) else (end - beg)
        scaled_num = max(1, int(round(float(base_flow.number) * scale * ((end - beg) / orig_dur))))
        elem.setAttribute("number", str(scaled_num))
    else:
        elem.setAttribute("vehsPerHour", format_num(3600.0 * scale))
    return elem


def main(args=None):
    options = get_options(args=args)
    static_objs = []
    base_flows = []
    tags = ("vType", "vTypeDistribution", "route", "routeDistribution", "flow")
    for obj in sumolib.xml.parse(options.routeFile, tags):
        if obj.name == "flow":
            base_flows.append(obj)
        else:
            static_objs.append(obj)

    if not base_flows:
        sys.exit("No <flow> elements found in '%s'." % options.routeFile)

    num_streams = len(base_flows)
    states = generate_gray_states(num_streams, len(options.levels), options.balanced)
    step_dur = float(sumolib.miscutils.parseTime(options.stepDuration))
    t0 = float(sumolib.miscutils.parseTime(options.begin))
    sub_dur = step_dur / float(options.substeps)

    if options.verbose:
        transitions = [0] * num_streams
        for i in range(len(states)):
            nxt = states[(i + 1) % len(states)]
            for s_idx in range(num_streams):
                if states[i][s_idx] != nxt[s_idx]:
                    transitions[s_idx] += 1
        print("Streams=%d Levels=%d States=%d Duration=%s CyclicTransitions=%s" % (
            num_streams, len(options.levels), len(states),
            format_num(len(states) * step_dur), transitions))

    with sumolib.openz(options.out, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "routes", options=options)
        for sobj in static_objs:
            outf.write(sobj.toXML("    "))
        sub_counter = 0
        for step_idx, state in enumerate(states):
            next_state = states[(step_idx + 1) % len(states)]
            for sub_idx in range(options.substeps):
                beg = t0 + (step_idx * step_dur) + (sub_idx * sub_dur)
                end = beg + sub_dur
                w = interp_weight(options.curve, (sub_idx + 0.5) / float(options.substeps))
                for f_idx, bflow in enumerate(base_flows):
                    lv0 = options.levels[state[f_idx]]
                    lv1 = options.levels[next_state[f_idx]]
                    scale = (1.0 - w) * lv0 + w * lv1
                    fid = "%s_%d" % (bflow.id, sub_counter)
                    felem = scale_flow_elem(bflow, fid, beg, end, scale)
                    if felem is not None:
                        outf.write(felem.toXML("    "))
                sub_counter += 1
        outf.write("</routes>\n")


if __name__ == "__main__":
    main()
