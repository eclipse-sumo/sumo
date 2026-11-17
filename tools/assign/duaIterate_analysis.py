#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    duaIterate_analysis.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2012-09-06

"""
Parses the log outputs from duaIterate.py.
By default, the concatenaded log of sumo outputs (named dua.log by default) is
parsed to obtain statistics on the following quantities:
inserted, teleports, waiting, loaded and running.
Optionally, the stdout.log is parsed as well to obtain statistics on
routingMinutes, simMinutes and absAvgError
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import re
import glob
from collections import defaultdict
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.miscutils import Statistics, uMax  # noqa
from sumolib.output import parse_fast  # noqa
from sumolib.options import ArgumentParser  # noqa
import sumolib.xml  # noqa


def parse_args():
    parser = ArgumentParser()
    parser.add_argument("--stdout", type=parser.file,
                        help="also parse the given FILE containing stdout of duaIterate")
    parser.add_argument("-o", "--output", category="output", default="plot",
                        help="output prefix for plotting with gnuplot")
    parser.add_argument("-l", "--label-size", default=40, dest="label_size", type=int,
                        help="limit length of the plot label to this size")
    parser.add_argument("--limit", type=int,
                        help="only parse the first INT number of iterations")
    parser.add_argument("--teleports", category="output", default="teleplot",
                        help="output prefix for plotting teleport-prone edges")
    parser.add_argument("--mpl", category="output",
                        help="output prefix for matplotlib plots or SHOW for plotting to the display")
    parser.add_argument("--alt-costs", dest="altCosts", action="store_true", default=False,
                        help="analyze distribution of route alternative costs from .rou.alt.xml files")
    parser.add_argument("--alt-dir", dest="altDir", category="input", type=parser.file,
                        help="directory to search for .rou.alt.xml(.gz) files (defaults to dualog directory)")
    parser.add_argument("--variance-threshold", dest="varThreshold", type=float,
                        help="report first iteration where mean alternative cost variance drops below FLOAT")
    parser.add_argument("dualog", category="output", nargs=1, type=parser.file, help="file path to dua log file")
    options = parser.parse_args()
    options.dualog = options.dualog[0]

    if options.varThreshold is not None:
        options.altCosts = True
    if options.limit is None:
        options.limit = uMax
    return options


def parse_dualog(dualog, limit):
    print("Parsing %s" % dualog)
    teleStats = Statistics('Teleports')
    header = ['#Inserted', 'Running', 'Waiting', 'Teleports', 'Loaded']
    step_values = []  # list of lists
    step_counts = []  # list of edge teleport counters
    reInserted = re.compile(r"Inserted: (\d*)")
    reLoaded = re.compile(r"Loaded: (\d*)")
    reRunning = re.compile(r"Running: (\d*)")
    reWaiting = re.compile(r"Waiting: (\d*)")
    reFrom = re.compile("from '([^']*)'")  # mesosim
    teleports = 0
    inserted = None
    loaded = None
    running = None
    waiting = None
    haveMicrosim = None
    counts = defaultdict(lambda: 0)
    with open(dualog) as dualogIn:
        for line in dualogIn:
            try:
                if "Warning: Teleporting vehicle" in line:
                    if haveMicrosim is None:
                        if "lane='" in line:
                            haveMicrosim = True
                            reFrom = re.compile("lane='([^']*)'")
                        else:
                            haveMicrosim = False
                    teleports += 1
                    edge = reFrom.search(line).group(1)
                    if ':' in edge:  # mesosim output
                        edge = edge.split(':')[0]
                    counts[edge] += 1
                elif "Inserted:" in line:
                    inserted = reInserted.search(line).group(1)
                    if "Loaded:" in line:  # optional output
                        loaded = reLoaded.search(line).group(1)
                    else:
                        loaded = inserted
                elif "Running:" in line:
                    running = reRunning.search(line).group(1)
                elif "Waiting:" in line:
                    iteration = len(step_values)
                    if iteration > limit:
                        break
                    waiting = reWaiting.search(line).group(1)
                    teleStats.add(teleports, iteration)
                    step_values.append([inserted, running, waiting, teleports, loaded])
                    teleports = 0
                    step_counts.append(counts)
                    counts = defaultdict(lambda: 0)
            except Exception:
                sys.exit("error when parsing line '%s'" % line)

    print("  parsed %s steps" % len(step_values))
    print(teleStats)
    return [header] + step_values, step_counts


def parse_stdout(step_values, stdout):
    print("Parsing %s" % stdout)
    step_values[0] += ['routingMinutes', 'simMinutes', 'absAvgError']
    reDuration = re.compile("Duration: (.*)$")
    reError = re.compile(r"Absolute Error avg:(\d*)")

    def get_minutes(line):
        hours, minutes, seconds = reDuration.search(line).group(1).split(':')
        return int(hours) * 60 + int(minutes) + float(seconds) / 60
    step = 1
    routingMinutes = None
    simMinutes = None
    error = None
    for line in open(stdout):
        if "Duration" in line:
            if routingMinutes is None:
                routingMinutes = get_minutes(line)
            else:
                simMinutes = get_minutes(line)
        elif "Absolute" in line:
            error = reError.search(line).group(1)
            step_values[step] += [routingMinutes, simMinutes, error]
            step += 1
            if step >= len(step_values):
                break
            routingMinutes = None
    print("  parsed %s steps" % (step - 1))


def gnuplot_teleport_edges(plotfile, step_counts, xlabel):
    datafile = plotfile + '.data'
    # an edge is interesting if a large proportion of teleports happen on it
    interestingness = defaultdict(lambda: 0)
    for counts in step_counts:
        teleports = float(sum(counts.values()))
        if teleports == 0:
            continue
        for edge, count in counts.items():
            interestingness[edge] += count / teleports
    interesting = sorted([(c, e) for e, c in interestingness.items()])[-7:]
    print("most interesting edges:", interesting)
    if len(interesting) > 0:
        interesting = [e for c, e in interesting]
        with open(datafile, 'w') as f:
            print('#' + ' '.join(interesting), file=f)
            for counts in step_counts:
                values = [counts[e] for e in interesting]
                f.write(' '.join(map(str, values)) + '\n')
        # write plotfile
        with open(plotfile, 'w') as f:
            f.write("set xlabel '%s'\nplot \\\n" % xlabel)
            lines = ["'%s' using 0:%s title '%s' with lines" % (datafile, ii + 1, edge)
                     for ii, edge in enumerate(interesting)]
            f.write(', \\\n'.join(lines))


def parse_trip_durations():
    result = []
    for file in sorted(glob.glob("tripinfo_*.xml")):
        result.append([float(t.duration)
                       for t in parse_fast(file, 'tripinfo', ['duration'])])
    return result


def discover_alt_files(alt_dir, limit):
    iterations = []
    subdirs = sorted([d for d in os.listdir(alt_dir)
                      if os.path.isdir(os.path.join(alt_dir, d)) and d.isdigit()],
                     key=lambda s: int(s))
    if subdirs:
        for d in subdirs:
            step = int(d)
            if step > limit:
                break
            files = sorted(glob.glob(os.path.join(alt_dir, d, "*.rou.alt.xml*")))
            if files:
                iterations.append((step, files))
    else:
        by_step = defaultdict(list)
        re_iter = re.compile(r"_(\d+)\.rou\.alt\.xml(?:\.gz)?$")
        for idx, f in enumerate(sorted(glob.glob(os.path.join(alt_dir, "*.rou.alt.xml*")))):
            m = re_iter.search(os.path.basename(f))
            step = int(m.group(1)) if m else idx
            if step <= limit:
                by_step[step].append(f)
        for step in sorted(by_step.keys()):
            iterations.append((step, by_step[step]))
    return iterations


def parse_alt_costs(alt_dir, limit, var_threshold=None):
    chosen_distributions = []
    reached_iter = None
    for step, files in discover_alt_files(alt_dir, limit):
        chosen_stats = Statistics("Iteration %s chosen cost" % step)
        var_stats = Statistics("Iteration %s alt cost variance" % step)
        step_chosen = []
        for fpath in files:
            for veh in sumolib.xml.parse(fpath, 'vehicle'):
                if not veh.routeDistribution or not veh.routeDistribution[0].route:
                    continue
                rd = veh.routeDistribution[0]
                routes = rd.route
                costs = [float(r.cost) for r in routes if r.cost is not None]
                if not costs:
                    continue
                last_idx = int(rd.last) if rd.last is not None else 0
                if 0 <= last_idx < len(costs):
                    chosen = costs[last_idx]
                else:
                    chosen = costs[-1]
                chosen_stats.add(chosen, veh.id)
                step_chosen.append(chosen)
                if len(costs) > 1:
                    mean_c = sum(costs) / len(costs)
                    var_c = sum((c - mean_c) ** 2 for c in costs) / len(costs)
                    var_stats.add(var_c, veh.id)
        chosen_distributions.append(step_chosen)
        print(chosen_stats)
        print(var_stats)
        if var_threshold is not None and reached_iter is None and var_stats.count() > 0:
            if var_stats.avg() <= var_threshold:
                reached_iter = (step, var_stats.avg())
    if var_threshold is not None:
        if reached_iter is not None:
            print("Variance threshold %s reached at iteration %s (mean variance=%.2f)" % (
                var_threshold, reached_iter[0], reached_iter[1]))
        else:
            print("Variance threshold %s not reached" % var_threshold)
    return chosen_distributions


def matplot(output, alt_distributions=None):
    if output is not None:
        # the following patches matplotlib for python 3.14
        from sumolib.visualization import helpers  # noqa
        import matplotlib
        if output != 'SHOW':
            matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        durations = parse_trip_durations()
        plt.boxplot(durations if durations or not alt_distributions else alt_distributions)
        if output == 'SHOW':
            plt.show()
        else:
            plt.savefig(output)
            plt.close()


def gnuplot_vehicle_summary(plotfile, xlabel, step_values):
    datafile = plotfile + '.data'
    with open(datafile, 'w') as f:
        for values in step_values:
            f.write(' '.join(map(str, values)) + '\n')

    with open(plotfile, 'w') as f:
        f.write("""
set xlabel '%s'
plot \\
'%s' using 0:1 title 'inserted' with lines, \\
'%s' using 0:4 title 'teleports' with lines, \\
'%s' using 0:3 title 'waiting' with lines, \\
'%s' using 0:5 title 'loaded' with lines, \\
'%s' using 0:2 title 'running' with lines
""" % ((xlabel,) + (datafile,) * 5))


def main():
    options = parse_args()
    step_values, step_counts = parse_dualog(options.dualog, options.limit)
    if options.stdout is not None:
        parse_stdout(step_values, options.stdout)
    duaPath = os.path.dirname(
        os.path.abspath(options.dualog))[-options.label_size:]
    xlabel = 'Iterations in ' + duaPath

    gnuplot_vehicle_summary(options.output, xlabel, step_values)
    gnuplot_teleport_edges(options.teleports, step_counts, xlabel)
    alt_distributions = None
    if options.altCosts:
        alt_dir = options.altDir if options.altDir is not None else (os.path.dirname(options.dualog) or ".")
        alt_distributions = parse_alt_costs(alt_dir, options.limit, options.varThreshold)
    matplot(options.mpl, alt_distributions)


##################
if __name__ == "__main__":
    main()
