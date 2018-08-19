#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    plotTrajectories.py
# @author  Jakob Erdmann
# @date    2018-08-18
# @version $Id$

"""
This script plots fcd data for each vehicle using either
- distance vs speed
- time vs speed
- time vs distance
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os
from collections import defaultdict
from optparse import OptionParser
import matplotlib.pyplot as plt
from xml.sax import handler, parse


def getOptions(args=None):
    optParser = OptionParser()
    optParser.add_option("-t", "--trajectory-type", dest="ttype", default="ds",
                         help="select one of ('ds','ts', 'td') for plotting distanceVsSpeed (default), timeVsSpeed, timeVsDistance")
    optParser.add_option("-s", "--show", action="store_true", default=False, help="show plot directly")
    optParser.add_option("-o", "--output", help="outputfile for saving plots", default="plot.png")
    optParser.add_option("--csv-output", dest="csv_output", help="write plot as csv", metavar="FILE")
    optParser.add_option("-b", "--ballistic", action="store_true", default=False, help="perform ballistic integration of distance")
    optParser.add_option("--filter-route", dest="filterRoute", help="only export trajectories that pass the given list of edges (regardless of gaps)")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")

    options, args = optParser.parse_args(args=args)
    if len(args) != 1:
        sys.exit("mandatory argument FCD_FILE missing")
    options.fcdfile = args[0]

    if options.filterRoute is not None:
        options.filterRoute = options.filterRoute.split(',')
    return options

def write_csv(data, fname):
    with open(fname, 'w') as f:
        for veh, vals in data.items():
            f.write('"%s"\n' % veh)
            for x in zip(*vals):
                f.write(" ".join(map(str, x)) + "\n")
            f.write('\n')


class FCDReader(handler.ContentHandler):
    def __init__(self, options):
        self._time = None
        self.routes = defaultdict(list) # vehID -> recorded edges
        self.data = defaultdict(lambda : ([], [], [])) # vehID -> (times, speeds, distances) 
        self._options = options

    def startElement(self, name, attrs):
        if name == 'timestep':
            self._time = float(attrs["time"])
        if name == 'vehicle':
            vehID = attrs["id"]
            speed = float(attrs["speed"])
            lane = attrs["lane"]
            prevTime = self._time
            prevSpeed = speed
            prevDist = 0
            if vehID in self.data:
                vehData = self.data[vehID]
                prevTime = vehData[0][-1]
                prevSpeed = vehData[1][-1]
                prevDist = vehData[2][-1]
            else:
                vehData = self.data[vehID]
            vehData[0].append(self._time)
            vehData[1].append(speed)

            if self._options.ballistic:
                avgSpeed = (speed + prevSpeed) / 2
            else:
                avgSpeed = speed

            vehData[2].append(prevDist + (self._time - prevTime) * avgSpeed)
            edge = lane[0:lane.rfind('_')]
            vehRoute = self.routes[vehID]
            if len(vehRoute) == 0 or vehRoute[-1] != edge:
                vehRoute.append(edge)


def main(options):
    plt.figure(figsize=(14, 9), dpi=100)

    xdata = 2
    ydata = 1
    if options.ttype == 'ds':
        plt.xlabel("Distance")
        plt.ylabel("Speed")
    elif options.ttype == 'ts':
        plt.xlabel("Time")
        plt.ylabel("Speed")
        xdata = 0
        ydata = 1
    elif options.ttype == 'td':
        plt.xlabel("Time")
        plt.ylabel("Distance")
        xdata = 0
        ydata = 2
    else:
        sys.exit("unsupported plot type '%s'" % options.ttype)

    fcdReader = FCDReader(options)
    parse(options.fcdfile, fcdReader)

    for vehID, d in fcdReader.data.items():
        if options.filterRoute is not None:
            skip = False
            route = fcdReader.routes[vehID]
            for required in options.filterRoute:
                if not required in route:
                    if vehID == "cg3_cg1_35.47":
                        print("skip cg3_cg1_35.47 due to required '%s' (route=%s)" % (required, route))
                    skip = True
                    break;
            if skip:
                continue
        plt.plot(d[xdata], d[ydata])

    plt.savefig(options.output)
    if options.csv_output is not None:
        write_csv(fcdReader.data, options.csv_output)
    if options.show:
        plt.show()

if __name__ == "__main__":
    main(getOptions())
