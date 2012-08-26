#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    duaIterate_analysis.py
@author  Jakob Erdmann
@date    2012-08-22
@version $Id$

Extract statistics from the outputs of a duaIterate run for plotting.
plotting example:
    plot \
            'duaIterate.gnuplot' using 0:1 title 'emitted',\
            'duaIterate.gnuplot' using 0:4 title 'teleports',\
            'duaIterate.gnuplot' using 0:3 title 'waiting',\
            'duaIterate.gnuplot' using 0:($6 * 60) title 'sim s'

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os,sys
import re
from optparse import OptionParser
from collections import defaultdict

def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <dua-log.txt> [options]"
    optParser = OptionParser()
    optParser.add_option("--stdout", 
            help="also parse the given FILE containing stdout of duaIterate")
    optParser.add_option("-o", "--output", default="duaIterate.gnuplot",
            help="output file for plotting with gnuplot")
    options, args = optParser.parse_args()
    if len(args) != 1:
        sys.exit(USAGE)
    options.dualog = args[0]
    return options

def parse_dualog(dualog):
    print "Parsing %s" % dualog
    step_values = [['#Emitted', 'Running', 'Waiting', 'Teleports']] # list of lists
    reEmitted = re.compile("Emitted: (\d*)")
    reRunning = re.compile("Running: (\d*)")
    reWaiting = re.compile("Waiting: (\d*)")
    teleports = 0
    emitted = None
    running = None
    waiting = None
    for line in open(dualog):
        if "Warning: Teleporting vehicle" in line:
            teleports += 1
        elif "Emitted:" in line:
            emitted = reEmitted.search(line).group(1)
        elif "Running:" in line:
            running = reRunning.search(line).group(1)
        elif "Waiting:" in line:
            waiting = reWaiting.search(line).group(1)
            step_values.append([emitted, running, waiting, teleports])
            teleports = 0
    print "  parsed %s steps" % (len(step_values) - 1)
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
            routingMinutes = None
    print "  parsed %s steps" % (step - 1)


def write(outfile, step_values):
    with open(outfile, 'w') as f:
        for values in step_values:
            f.write(' '.join(map(str,values)) + '\n')

def main():
    options = parse_args()
    step_values = parse_dualog(options.dualog)
    if options.stdout is not None:
        parse_stdout(step_values, options.stdout)
    write(options.output, step_values)

##################
if __name__ == "__main__":
    main()
