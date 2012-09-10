#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    duaIterate_analysis.py
@author  Jakob Erdmann
@date    2012-09-06
@version $Id$

Extract statistics from the outputs of a duaIterate run for plotting.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
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
    options, args = optParser.parse_args()
    if len(args) != 1:
        sys.exit(USAGE)
    options.dualog = args[0]
    return options

def parse_dualog(dualog, limit):
    print "Parsing %s" % dualog
    teleStats = Statistics('Teleports')
    step_values = [['#Emitted', 'Running', 'Waiting', 'Teleports', 'Loaded']] # list of lists
    reEmitted = re.compile("Emitted: (\d*)")
    reLoaded = re.compile("Loaded: (\d*)")
    reRunning = re.compile("Running: (\d*)")
    reWaiting = re.compile("Waiting: (\d*)")
    teleports = 0
    emitted = None
    loaded = None
    running = None
    waiting = None
    for line in open(dualog):
        if "Warning: Teleporting vehicle" in line:
            teleports += 1
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
    print "  parsed %s steps" % (len(step_values) - 1)
    print teleStats
    return step_values

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


def main():
    options = parse_args()
    plotfile = options.output
    datafile = plotfile + '.data'
    step_values = parse_dualog(options.dualog, options.limit)
    if options.stdout is not None:
        parse_stdout(step_values, options.stdout)
    write_data(datafile, step_values)
    duaPath = os.path.dirname(os.path.abspath(options.dualog))[-options.label_size:]
    write_plotfile(plotfile, datafile, 'Iterations in ' + duaPath)

##################
if __name__ == "__main__":
    main()
