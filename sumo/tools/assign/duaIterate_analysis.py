#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    duaIterate_analysis.py
@author  Jakob Erdmann
@date    2012-09-06
@version $Id$

Extract statistics from the outputs of a duaIterate run for plotting.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os,sys
import re
from optparse import OptionParser
from collections import defaultdict
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from sumolib.miscutils import Statistics, uMax

def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <dua-log.txt> [options]"
    optParser = OptionParser()
    optParser.add_option("--stdout", 
            help="also parse the given FILE containing stdout of duaIterate")
    optParser.add_option("-o", "--output", default="plot",
            help="output prefix for plotting with gnuplot")
    optParser.add_option("-l", "--label-size", default=40, dest="label_size",
            help="limit length of the plot label to this size")
    optParser.add_option("--limit", type=int,  default=uMax,
            help="only parse the first INT number of iterations")
    optParser.add_option("--teleports", default="teleplot",
            help="output prefix for plotting teleport-prone edges")
    options, args = optParser.parse_args()
    if len(args) != 1:
        sys.exit(USAGE)
    options.dualog = args[0]
    return options

def parse_dualog(dualog, limit):
    print "Parsing %s" % dualog
    teleStats = Statistics('Teleports')
    header = ['#Emitted', 'Running', 'Waiting', 'Teleports', 'Loaded']
    step_values = [] # list of lists
    step_counts = [] # list of edge teleport counters
    reEmitted = re.compile("Emitted: (\d*)")
    reLoaded = re.compile("Loaded: (\d*)")
    reRunning = re.compile("Running: (\d*)")
    reWaiting = re.compile("Waiting: (\d*)")
    reFrom = re.compile("from '([^']*)'") # mesosim
    teleports = 0
    emitted = None
    loaded = None
    running = None
    waiting = None
    haveMicrosim = None
    counts = defaultdict(lambda:0)
    for line in open(dualog):
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
                if ':' in edge: # mesosim output
                    edge = edge.split(':')[0]
                counts[edge] += 1
            elif "Emitted:" in line:
                emitted = reEmitted.search(line).group(1)
                if "Loaded:" in line: # optional output
                    loaded = reLoaded.search(line).group(1)
                else:
                    loaded = emitted
            elif "Running:" in line:
                running = reRunning.search(line).group(1)
            elif "Waiting:" in line:
                iteration = len(step_values)
                if iteration > limit:
                    break
                waiting = reWaiting.search(line).group(1)
                teleStats.add(teleports, iteration)
                step_values.append([emitted, running, waiting, teleports, loaded])
                teleports = 0
                step_counts.append(counts)
                counts = defaultdict(lambda:0)
        except:
            sys.exit("error when parsing line '%s'" % line)

    print "  parsed %s steps" % len(step_values)
    print teleStats
    return [header] + step_values, step_counts

def parse_stdout(step_values, stdout):
    print "Parsing %s" % stdout
    step_values[0] += ['routingMinutes', 'simMinutes', 'absAvgError']
    reDuration = re.compile("Duration: (.*)$")
    reError = re.compile("Absolute Error avg:(\d*)")
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
    print "  parsed %s steps" % (step - 1)


def write_data(outfile, step_values):
    with open(outfile, 'w') as f:
        for values in step_values:
            f.write(' '.join(map(str,values)) + '\n')

def write_plotfile(outfile, datafile, xlabel):
    with open(outfile, 'w') as f:
        f.write("""
set xlabel '%s'
plot \\
'%s' using 0:1 title 'emitted' with lines, \\
'%s' using 0:4 title 'teleports' with lines, \\
'%s' using 0:3 title 'waiting' with lines, \\
'%s' using 0:5 title 'loaded' with lines, \\
'%s' using 0:2 title 'running' with lines 
""" % ((xlabel,) + (datafile,) * 5))


def create_teleplot(plotfile, step_counts, xlabel):
    datafile = plotfile + '.data'
    # an edge is interesting if a large proportion of teleports happen on it
    interestingness = defaultdict(lambda:0)
    all_edges = set()
    for counts in step_counts:
        teleports = float(sum(counts.itervalues()))
        if teleports == 0:
            continue
        for edge, count in counts.iteritems():
            interestingness[edge] += count/teleports
    interesting = sorted([(c,e) for e,c in interestingness.iteritems()])[-7:]
    print "most interesting edges:", interesting
    interesting = [e for c,e in interesting]
    with open(datafile, 'w') as f:
        print >>f, '#' + ' '.join(interesting)
        for counts in step_counts:
            values = [counts[e] for e in interesting]
            f.write(' '.join(map(str,values)) + '\n')
    # write plotfile
    with open(plotfile, 'w') as f:
        f.write("set xlabel '%s'\nplot \\\n")
        lines = ["'%s' using 0:%s title '%s' with lines" % (datafile, ii+1, edge) 
                for ii, edge in enumerate(interesting)]
        f.write(', \\\n'.join(lines))


def main():
    options = parse_args()
    plotfile = options.output
    datafile = plotfile + '.data'
    step_values, step_counts = parse_dualog(options.dualog, options.limit)
    if options.stdout is not None:
        parse_stdout(step_values, options.stdout)
    write_data(datafile, step_values)
    duaPath = os.path.dirname(os.path.abspath(options.dualog))[-options.label_size:]
    xlabel = 'Iterations in ' + duaPath
    write_plotfile(plotfile, datafile, xlabel)
    create_teleplot(options.teleports, step_counts, xlabel)

##################
if __name__ == "__main__":
    main()
